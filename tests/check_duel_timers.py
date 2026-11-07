"""Compile exact native timer methods with minimal hosts for otherwise unreachable bank edges."""
import argparse
import hashlib
import json
import os
import shlex
import subprocess
from pathlib import Path
from xml.sax.saxutils import escape


def extract(source, signature):
    assert source.count(signature) == 1, signature
    start = source.index(signature)
    depth = 0
    for index in range(source.index('{', start), len(source)):
        if source[index] == '{':
            depth += 1
        elif source[index] == '}':
            depth -= 1
            if not depth:
                return source[start:index + 1]
    raise ValueError('Unterminated method: ' + signature)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--msbuild', type=Path, help='MSBuild.exe for Windows; otherwise use CXX/c++')
    args = parser.parse_args()
    root = args.output_dir.resolve()
    root.mkdir(parents=True, exist_ok=True)
    methods = []
    hashes = {}
    for stem, name in [('single_duel', 'SingleDuel'), ('tag_duel', 'TagDuel')]:
        path = args.source / 'gframe' / (stem + '.cpp')
        source = path.read_text(encoding='utf-8-sig')
        hashes[str(path.resolve())] = hashlib.sha256(path.read_bytes()).hexdigest()
        for suffix in ['WaitforResponse(int playerid)', 'TimeConfirm(DuelPlayer* dp)', 'TimerTick()']:
            methods.append(extract(source, f'void {name}::{suffix}'))
    stub = r'''
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>
#ifdef NDEBUG
#error Timer acceptance requires active assertions
#endif
enum { CTOS_RESPONSE=1, CTOS_TIME_CONFIRM=21, STOC_TIME_LIMIT=24, STOC_GAME_MSG=1, MSG_WAITING=3, MSG_WIN=5 };
struct DuelPlayer { unsigned char state=0xff; unsigned char type=0; };
struct STOC_TimeLimit { unsigned char player; uint16_t left_time; };
struct NetServer {
    static int packets;
    template<class T> static void SendPacketToPlayer(DuelPlayer*,int,const T&) { ++packets; }
    static void SendBufferToPlayer(DuelPlayer*,int,unsigned char*,int) { ++packets; }
    static void ReSendToPlayer(DuelPlayer*) {}
};
int NetServer::packets=0;
struct TimerHost {
    uintptr_t pduel=1;
    struct { uint16_t time_limit=180; } host_info;
    DuelPlayer seats[4];
    DuelPlayer* players[4];
    DuelPlayer* pplayer[4];
    DuelPlayer* cur_player[2];
    std::vector<DuelPlayer*> observers;
    unsigned char last_response=0, duel_count=0, tp_player=0, match_result[5]{};
    uint16_t time_limit[2]{1,1}, time_elapsed=0, time_confirm_elapsed=0;
    bool awaiting_time_confirm=false;
    int ended=0, completed=0;
    TimerHost() {
        for(int i=0;i<4;i++) { seats[i].type=static_cast<unsigned char>(i); players[i]=pplayer[i]=&seats[i]; }
        cur_player[0]=players[0]; cur_player[1]=players[3];
    }
    void EndDuel() { pduel=0; ++ended; }
    void DuelEndProc() { ++completed; }
};
struct SingleDuel : TimerHost { void WaitforResponse(int); void TimeConfirm(DuelPlayer*); void TimerTick(); };
struct TagDuel : TimerHost { void WaitforResponse(int); void TimeConfirm(DuelPlayer*); void TimerTick(); };
'''
    tests = r'''
template<class Duel> void check(const char* name) {
    { Duel d; for(int i=0;i<20;i++)d.TimerTick(); assert(d.time_elapsed==0 && d.ended==0); }
    { Duel d; d.WaitforResponse(0); for(int i=0;i<12;i++)d.TimerTick();
      assert(d.time_elapsed==0 && d.time_limit[0]==1 && d.ended==0);
      d.TimeConfirm(d.players[1]); assert(d.awaiting_time_confirm);
      int packets=NetServer::packets; d.TimeConfirm(d.players[0]);
      assert(!d.awaiting_time_confirm && d.time_elapsed==0 && NetServer::packets==packets);
      d.TimerTick(); assert(d.ended==1 && d.completed==1); }
    { Duel d; d.time_limit[0]=3; d.WaitforResponse(0); d.TimeConfirm(d.players[0]); d.TimerTick();
      d.TimeConfirm(d.players[0]); assert(d.time_elapsed==1); d.TimerTick(); assert(d.ended==0);
      d.TimerTick(); assert(d.ended==1); }
    { Duel d; d.time_limit[0]=0; d.WaitforResponse(0); for(int i=0;i<12;i++)d.TimerTick();
      assert(d.ended==0); d.TimeConfirm(d.players[0]); assert(d.ended==1 && d.pduel==0); }
    { Duel d; d.time_limit[0]=180; d.WaitforResponse(0); for(int i=0;i<59;i++)d.TimerTick();
      assert(d.ended==0 && d.time_limit[0]==180); d.TimerTick(); assert(d.ended==1); }
    { Duel d; d.host_info.time_limit=0; d.WaitforResponse(0); for(int i=0;i<70;i++)d.TimerTick();
      assert(d.ended==0 && d.time_elapsed==0); }
    { Duel d; d.WaitforResponse(0); d.TimeConfirm(d.players[0]); d.players[0]->state=0xff;
      for(int i=0;i<12;i++)d.TimerTick(); assert(d.time_elapsed==0 && d.ended==0); }
    std::cout << "PASS " << name << " 7 cases\n";
}
int main() { check<SingleDuel>("SingleDuel"); check<TagDuel>("TagDuel"); }
'''
    cpp = root / 'NativeTimerStateTests.cpp'
    cpp.write_text(stub + '\n'.join(methods) + tests, encoding='utf-8')
    project = root / 'NativeTimerStateTests.vcxproj'
    project.write_text('''<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
<ItemGroup Label="ProjectConfigurations"><ProjectConfiguration Include="Release|x64"><Configuration>Release</Configuration><Platform>x64</Platform></ProjectConfiguration></ItemGroup>
<PropertyGroup Label="Globals"><ProjectGuid>{A579C452-7908-419B-A063-65D238AA75C1}</ProjectGuid><WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion></PropertyGroup>
<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props"/>
<PropertyGroup Label="Configuration"><ConfigurationType>Application</ConfigurationType><PlatformToolset>v143</PlatformToolset></PropertyGroup>
<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.props"/>
<PropertyGroup><OutDir>$(ProjectDir)bin\\</OutDir><IntDir>$(ProjectDir)obj\\</IntDir></PropertyGroup>
<ItemDefinitionGroup><ClCompile><RuntimeLibrary>MultiThreaded</RuntimeLibrary><PreprocessorDefinitions>%(PreprocessorDefinitions)</PreprocessorDefinitions></ClCompile></ItemDefinitionGroup>
<ItemGroup><ClCompile Include="''' + escape(str(cpp)) + '''"/></ItemGroup><Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets"/></Project>''', encoding='utf-8')
    if args.msbuild:
        executable = root / 'bin/NativeTimerStateTests.exe'
        command = [str(args.msbuild.resolve()), str(project), '/p:Configuration=Release', '/p:Platform=x64', '/nr:false', '/nologo', '/v:minimal']
    else:
        executable = root / ('NativeTimerStateTests.exe' if os.name == 'nt' else 'NativeTimerStateTests')
        command = shlex.split(os.environ.get('CXX', 'c++')) + ['-std=c++17', '-UNDEBUG', str(cpp), '-o', str(executable)]
    built = subprocess.run(command, capture_output=True, text=True)
    (root / 'build.log').write_text(built.stdout + built.stderr, encoding='utf-8')
    print(built.stdout + built.stderr)
    built.check_returncode()
    checked = subprocess.run([str(executable)], capture_output=True, text=True)
    print(checked.stdout + checked.stderr)
    (root / 'source-method-results.json').write_text(json.dumps(dict(
        sourceHashes=hashes, cases=14, command=command, returnCode=checked.returncode,
        output=checked.stdout + checked.stderr,
    ), indent=2), encoding='utf-8')
    checked.check_returncode()
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
