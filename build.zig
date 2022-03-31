const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardReleaseOptions();

    const c_flags = [_][]const u8{
        // https://github.com/ziglang/zig/wiki/FAQ#why-do-i-get-illegal-instruction-when-using-with-zig-cc-to-build-c-code
        // Currently required because the codebase seems to contain some UB.
        // Can be commented out for debugging.
        "-fno-sanitize=undefined",

        "-DNDEBUG",
        "-Wno-format-security",
        "-fexceptions",
        "-fno-strict-aliasing",
        "-fomit-frame-pointer",
    };

    const cxx_flags = c_flags ++ [_][]const u8{
        "-fno-rtti",
        "-std=c++14",
    };

    const lzma = b.addStaticLibrary("lzma", null);
    lzma.addCSourceFiles(&.{
        "gframe/lzma/Alloc.c",
        "gframe/lzma/LzFind.c",
        "gframe/lzma/LzmaDec.c",
        "gframe/lzma/LzmaEnc.c",
        "gframe/lzma/LzmaLib.c",
    }, &c_flags);

    lzma.linkLibC();

    const ocgcore = b.addStaticLibrary("ocgcore", null);
    ocgcore.addCSourceFiles(&.{
        "ocgcore/card.cpp",
        "ocgcore/duel.cpp",
        "ocgcore/effect.cpp",
        "ocgcore/field.cpp",
        "ocgcore/group.cpp",
        "ocgcore/interpreter.cpp",
        "ocgcore/libcard.cpp",
        "ocgcore/libdebug.cpp",
        "ocgcore/libduel.cpp",
        "ocgcore/libeffect.cpp",
        "ocgcore/libgroup.cpp",
        "ocgcore/mem.cpp",
        "ocgcore/ocgapi.cpp",
        "ocgcore/operations.cpp",
        "ocgcore/playerop.cpp",
        "ocgcore/processor.cpp",
        "ocgcore/scriptlib.cpp",
    }, &cxx_flags);

    ocgcore.addIncludeDir("lua-5.3.5/src");

    ocgcore.linkLibCpp();

    const exe = b.addExecutable("ygopro", null);
    exe.addCSourceFile("gframe/spmemvfs/spmemvfs.c", &c_flags);

    exe.addCSourceFiles(&.{
        "gframe/CGUIImageButton.cpp",
        "gframe/CGUITTFont.cpp",
        "gframe/client_card.cpp",
        "gframe/client_field.cpp",
        "gframe/data_manager.cpp",
        "gframe/deck_con.cpp",
        "gframe/deck_manager.cpp",
        "gframe/drawing.cpp",
        "gframe/duelclient.cpp",
        "gframe/event_handler.cpp",
        "gframe/game.cpp",
        "gframe/gframe.cpp",
        "gframe/image_manager.cpp",
        "gframe/materials.cpp",
        "gframe/menu_handler.cpp",
        "gframe/netserver.cpp",
        "gframe/replay.cpp",
        "gframe/replay_mode.cpp",
        "gframe/single_duel.cpp",
        "gframe/single_mode.cpp",
        "gframe/sound_manager.cpp",
        "gframe/tag_duel.cpp",
    }, &cxx_flags);

    exe.addLibPath("lua-5.3.5/src");

    exe.linkLibCpp();
    exe.linkLibrary(lzma);
    exe.linkLibrary(ocgcore);
    exe.linkSystemLibrary("GL");
    exe.linkSystemLibrary("Irrlicht");
    exe.linkSystemLibrary("event");
    exe.linkSystemLibrary("event_pthreads");
    exe.linkSystemLibrary("freetype");
    exe.linkSystemLibrary("lua");
    exe.linkSystemLibrary("sqlite3");

    exe.setTarget(target);
    exe.setBuildMode(mode);

    exe.install();

    const run_cmd = exe.run();
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
