##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=gframe
ConfigurationName      :=Release
IntermediateDirectory  :=../Release
OutDir                 := $(IntermediateDirectory)
WorkspacePath          := "E:\ygo"
ProjectPath            := "E:\ygo\gframe"
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=argon
Date                   :=2011/12/8
CodeLitePath           :="C:\Program Files (x86)\CodeLite"
LinkerName             :=g++
ArchiveTool            :=ar rcus
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
CompilerName           :=g++
C_CompilerName         :=gcc
OutputFile             :=../$(ProjectName).exe
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="E:\ygo\gframe\gframe.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
CmpOptions             := -std=gnu++0x $(Preprocessors)
C_CmpOptions           :=  $(Preprocessors)
RcCmpOptions           := 
RcCompilerName         :=windres
LinkOptions            :=  ../res.o -O2 -Wall -s -mwindows
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)lua $(LibrarySwitch)Irrlicht $(LibrarySwitch)freetype $(LibrarySwitch)sqlite3 $(LibrarySwitch)glu32 $(LibrarySwitch)opengl32 $(LibrarySwitch)winmm $(LibrarySwitch)gdi32 $(LibrarySwitch)Ws2_32 $(LibrarySwitch)imm32 
LibPath                := $(LibraryPathSwitch). 


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files (x86)\CodeLite
Objects=$(IntermediateDirectory)/gframe$(ObjectSuffix) $(IntermediateDirectory)/game$(ObjectSuffix) $(IntermediateDirectory)/data_manager$(ObjectSuffix) $(IntermediateDirectory)/image_manager$(ObjectSuffix) $(IntermediateDirectory)/CGUITTFont$(ObjectSuffix) $(IntermediateDirectory)/event_handler$(ObjectSuffix) $(IntermediateDirectory)/CGUIImageButton$(ObjectSuffix) $(IntermediateDirectory)/drawing$(ObjectSuffix) $(IntermediateDirectory)/network$(ObjectSuffix) $(IntermediateDirectory)/deck_manager$(ObjectSuffix) \
	$(IntermediateDirectory)/message$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_card$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_duel$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_effect$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_field$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_group$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_interpreter$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_libcard$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_libduel$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_libeffect$(ObjectSuffix) \
	$(IntermediateDirectory)/ocgcore_libgroup$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_mem$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_ocgapi$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_operations$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_playerop$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_processor$(ObjectSuffix) $(IntermediateDirectory)/ocgcore_scriptlib$(ObjectSuffix) $(IntermediateDirectory)/client_card$(ObjectSuffix) $(IntermediateDirectory)/client_field$(ObjectSuffix) $(IntermediateDirectory)/deck_con$(ObjectSuffix) \
	$(IntermediateDirectory)/replay$(ObjectSuffix) $(IntermediateDirectory)/lzma_Alloc$(ObjectSuffix) $(IntermediateDirectory)/lzma_LzFind$(ObjectSuffix) $(IntermediateDirectory)/lzma_LzmaDec$(ObjectSuffix) $(IntermediateDirectory)/lzma_LzmaEnc$(ObjectSuffix) $(IntermediateDirectory)/lzma_LzmaLib$(ObjectSuffix) $(IntermediateDirectory)/CIrrDeviceWin32$(ObjectSuffix) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects) > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "../Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/gframe$(ObjectSuffix): gframe.cpp $(IntermediateDirectory)/gframe$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/gframe.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/gframe$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/gframe$(DependSuffix): gframe.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/gframe$(ObjectSuffix) -MF$(IntermediateDirectory)/gframe$(DependSuffix) -MM "E:/ygo/gframe/gframe.cpp"

$(IntermediateDirectory)/gframe$(PreprocessSuffix): gframe.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/gframe$(PreprocessSuffix) "E:/ygo/gframe/gframe.cpp"

$(IntermediateDirectory)/game$(ObjectSuffix): game.cpp $(IntermediateDirectory)/game$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/game.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/game$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/game$(DependSuffix): game.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/game$(ObjectSuffix) -MF$(IntermediateDirectory)/game$(DependSuffix) -MM "E:/ygo/gframe/game.cpp"

$(IntermediateDirectory)/game$(PreprocessSuffix): game.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/game$(PreprocessSuffix) "E:/ygo/gframe/game.cpp"

$(IntermediateDirectory)/data_manager$(ObjectSuffix): data_manager.cpp $(IntermediateDirectory)/data_manager$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/data_manager.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/data_manager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/data_manager$(DependSuffix): data_manager.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/data_manager$(ObjectSuffix) -MF$(IntermediateDirectory)/data_manager$(DependSuffix) -MM "E:/ygo/gframe/data_manager.cpp"

$(IntermediateDirectory)/data_manager$(PreprocessSuffix): data_manager.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/data_manager$(PreprocessSuffix) "E:/ygo/gframe/data_manager.cpp"

$(IntermediateDirectory)/image_manager$(ObjectSuffix): image_manager.cpp $(IntermediateDirectory)/image_manager$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/image_manager.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/image_manager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/image_manager$(DependSuffix): image_manager.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/image_manager$(ObjectSuffix) -MF$(IntermediateDirectory)/image_manager$(DependSuffix) -MM "E:/ygo/gframe/image_manager.cpp"

$(IntermediateDirectory)/image_manager$(PreprocessSuffix): image_manager.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/image_manager$(PreprocessSuffix) "E:/ygo/gframe/image_manager.cpp"

$(IntermediateDirectory)/CGUITTFont$(ObjectSuffix): CGUITTFont.cpp $(IntermediateDirectory)/CGUITTFont$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/CGUITTFont.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/CGUITTFont$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CGUITTFont$(DependSuffix): CGUITTFont.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CGUITTFont$(ObjectSuffix) -MF$(IntermediateDirectory)/CGUITTFont$(DependSuffix) -MM "E:/ygo/gframe/CGUITTFont.cpp"

$(IntermediateDirectory)/CGUITTFont$(PreprocessSuffix): CGUITTFont.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CGUITTFont$(PreprocessSuffix) "E:/ygo/gframe/CGUITTFont.cpp"

$(IntermediateDirectory)/event_handler$(ObjectSuffix): event_handler.cpp $(IntermediateDirectory)/event_handler$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/event_handler.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/event_handler$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/event_handler$(DependSuffix): event_handler.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/event_handler$(ObjectSuffix) -MF$(IntermediateDirectory)/event_handler$(DependSuffix) -MM "E:/ygo/gframe/event_handler.cpp"

$(IntermediateDirectory)/event_handler$(PreprocessSuffix): event_handler.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/event_handler$(PreprocessSuffix) "E:/ygo/gframe/event_handler.cpp"

$(IntermediateDirectory)/CGUIImageButton$(ObjectSuffix): CGUIImageButton.cpp $(IntermediateDirectory)/CGUIImageButton$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/CGUIImageButton.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/CGUIImageButton$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CGUIImageButton$(DependSuffix): CGUIImageButton.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CGUIImageButton$(ObjectSuffix) -MF$(IntermediateDirectory)/CGUIImageButton$(DependSuffix) -MM "E:/ygo/gframe/CGUIImageButton.cpp"

$(IntermediateDirectory)/CGUIImageButton$(PreprocessSuffix): CGUIImageButton.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CGUIImageButton$(PreprocessSuffix) "E:/ygo/gframe/CGUIImageButton.cpp"

$(IntermediateDirectory)/drawing$(ObjectSuffix): drawing.cpp $(IntermediateDirectory)/drawing$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/drawing.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/drawing$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/drawing$(DependSuffix): drawing.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/drawing$(ObjectSuffix) -MF$(IntermediateDirectory)/drawing$(DependSuffix) -MM "E:/ygo/gframe/drawing.cpp"

$(IntermediateDirectory)/drawing$(PreprocessSuffix): drawing.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/drawing$(PreprocessSuffix) "E:/ygo/gframe/drawing.cpp"

$(IntermediateDirectory)/network$(ObjectSuffix): network.cpp $(IntermediateDirectory)/network$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/network.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/network$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/network$(DependSuffix): network.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/network$(ObjectSuffix) -MF$(IntermediateDirectory)/network$(DependSuffix) -MM "E:/ygo/gframe/network.cpp"

$(IntermediateDirectory)/network$(PreprocessSuffix): network.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/network$(PreprocessSuffix) "E:/ygo/gframe/network.cpp"

$(IntermediateDirectory)/deck_manager$(ObjectSuffix): deck_manager.cpp $(IntermediateDirectory)/deck_manager$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/deck_manager.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/deck_manager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/deck_manager$(DependSuffix): deck_manager.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/deck_manager$(ObjectSuffix) -MF$(IntermediateDirectory)/deck_manager$(DependSuffix) -MM "E:/ygo/gframe/deck_manager.cpp"

$(IntermediateDirectory)/deck_manager$(PreprocessSuffix): deck_manager.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/deck_manager$(PreprocessSuffix) "E:/ygo/gframe/deck_manager.cpp"

$(IntermediateDirectory)/message$(ObjectSuffix): message.cpp $(IntermediateDirectory)/message$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/message.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/message$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/message$(DependSuffix): message.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/message$(ObjectSuffix) -MF$(IntermediateDirectory)/message$(DependSuffix) -MM "E:/ygo/gframe/message.cpp"

$(IntermediateDirectory)/message$(PreprocessSuffix): message.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/message$(PreprocessSuffix) "E:/ygo/gframe/message.cpp"

$(IntermediateDirectory)/ocgcore_card$(ObjectSuffix): ../ocgcore/card.cpp $(IntermediateDirectory)/ocgcore_card$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/card.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_card$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_card$(DependSuffix): ../ocgcore/card.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_card$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_card$(DependSuffix) -MM "E:/ygo/ocgcore/card.cpp"

$(IntermediateDirectory)/ocgcore_card$(PreprocessSuffix): ../ocgcore/card.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_card$(PreprocessSuffix) "E:/ygo/ocgcore/card.cpp"

$(IntermediateDirectory)/ocgcore_duel$(ObjectSuffix): ../ocgcore/duel.cpp $(IntermediateDirectory)/ocgcore_duel$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/duel.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_duel$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_duel$(DependSuffix): ../ocgcore/duel.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_duel$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_duel$(DependSuffix) -MM "E:/ygo/ocgcore/duel.cpp"

$(IntermediateDirectory)/ocgcore_duel$(PreprocessSuffix): ../ocgcore/duel.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_duel$(PreprocessSuffix) "E:/ygo/ocgcore/duel.cpp"

$(IntermediateDirectory)/ocgcore_effect$(ObjectSuffix): ../ocgcore/effect.cpp $(IntermediateDirectory)/ocgcore_effect$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/effect.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_effect$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_effect$(DependSuffix): ../ocgcore/effect.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_effect$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_effect$(DependSuffix) -MM "E:/ygo/ocgcore/effect.cpp"

$(IntermediateDirectory)/ocgcore_effect$(PreprocessSuffix): ../ocgcore/effect.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_effect$(PreprocessSuffix) "E:/ygo/ocgcore/effect.cpp"

$(IntermediateDirectory)/ocgcore_field$(ObjectSuffix): ../ocgcore/field.cpp $(IntermediateDirectory)/ocgcore_field$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/field.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_field$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_field$(DependSuffix): ../ocgcore/field.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_field$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_field$(DependSuffix) -MM "E:/ygo/ocgcore/field.cpp"

$(IntermediateDirectory)/ocgcore_field$(PreprocessSuffix): ../ocgcore/field.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_field$(PreprocessSuffix) "E:/ygo/ocgcore/field.cpp"

$(IntermediateDirectory)/ocgcore_group$(ObjectSuffix): ../ocgcore/group.cpp $(IntermediateDirectory)/ocgcore_group$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/group.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_group$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_group$(DependSuffix): ../ocgcore/group.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_group$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_group$(DependSuffix) -MM "E:/ygo/ocgcore/group.cpp"

$(IntermediateDirectory)/ocgcore_group$(PreprocessSuffix): ../ocgcore/group.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_group$(PreprocessSuffix) "E:/ygo/ocgcore/group.cpp"

$(IntermediateDirectory)/ocgcore_interpreter$(ObjectSuffix): ../ocgcore/interpreter.cpp $(IntermediateDirectory)/ocgcore_interpreter$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/interpreter.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_interpreter$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_interpreter$(DependSuffix): ../ocgcore/interpreter.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_interpreter$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_interpreter$(DependSuffix) -MM "E:/ygo/ocgcore/interpreter.cpp"

$(IntermediateDirectory)/ocgcore_interpreter$(PreprocessSuffix): ../ocgcore/interpreter.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_interpreter$(PreprocessSuffix) "E:/ygo/ocgcore/interpreter.cpp"

$(IntermediateDirectory)/ocgcore_libcard$(ObjectSuffix): ../ocgcore/libcard.cpp $(IntermediateDirectory)/ocgcore_libcard$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/libcard.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_libcard$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_libcard$(DependSuffix): ../ocgcore/libcard.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_libcard$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_libcard$(DependSuffix) -MM "E:/ygo/ocgcore/libcard.cpp"

$(IntermediateDirectory)/ocgcore_libcard$(PreprocessSuffix): ../ocgcore/libcard.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_libcard$(PreprocessSuffix) "E:/ygo/ocgcore/libcard.cpp"

$(IntermediateDirectory)/ocgcore_libduel$(ObjectSuffix): ../ocgcore/libduel.cpp $(IntermediateDirectory)/ocgcore_libduel$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/libduel.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_libduel$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_libduel$(DependSuffix): ../ocgcore/libduel.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_libduel$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_libduel$(DependSuffix) -MM "E:/ygo/ocgcore/libduel.cpp"

$(IntermediateDirectory)/ocgcore_libduel$(PreprocessSuffix): ../ocgcore/libduel.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_libduel$(PreprocessSuffix) "E:/ygo/ocgcore/libduel.cpp"

$(IntermediateDirectory)/ocgcore_libeffect$(ObjectSuffix): ../ocgcore/libeffect.cpp $(IntermediateDirectory)/ocgcore_libeffect$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/libeffect.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_libeffect$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_libeffect$(DependSuffix): ../ocgcore/libeffect.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_libeffect$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_libeffect$(DependSuffix) -MM "E:/ygo/ocgcore/libeffect.cpp"

$(IntermediateDirectory)/ocgcore_libeffect$(PreprocessSuffix): ../ocgcore/libeffect.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_libeffect$(PreprocessSuffix) "E:/ygo/ocgcore/libeffect.cpp"

$(IntermediateDirectory)/ocgcore_libgroup$(ObjectSuffix): ../ocgcore/libgroup.cpp $(IntermediateDirectory)/ocgcore_libgroup$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/libgroup.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_libgroup$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_libgroup$(DependSuffix): ../ocgcore/libgroup.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_libgroup$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_libgroup$(DependSuffix) -MM "E:/ygo/ocgcore/libgroup.cpp"

$(IntermediateDirectory)/ocgcore_libgroup$(PreprocessSuffix): ../ocgcore/libgroup.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_libgroup$(PreprocessSuffix) "E:/ygo/ocgcore/libgroup.cpp"

$(IntermediateDirectory)/ocgcore_mem$(ObjectSuffix): ../ocgcore/mem.cpp $(IntermediateDirectory)/ocgcore_mem$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/mem.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_mem$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_mem$(DependSuffix): ../ocgcore/mem.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_mem$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_mem$(DependSuffix) -MM "E:/ygo/ocgcore/mem.cpp"

$(IntermediateDirectory)/ocgcore_mem$(PreprocessSuffix): ../ocgcore/mem.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_mem$(PreprocessSuffix) "E:/ygo/ocgcore/mem.cpp"

$(IntermediateDirectory)/ocgcore_ocgapi$(ObjectSuffix): ../ocgcore/ocgapi.cpp $(IntermediateDirectory)/ocgcore_ocgapi$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/ocgapi.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_ocgapi$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_ocgapi$(DependSuffix): ../ocgcore/ocgapi.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_ocgapi$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_ocgapi$(DependSuffix) -MM "E:/ygo/ocgcore/ocgapi.cpp"

$(IntermediateDirectory)/ocgcore_ocgapi$(PreprocessSuffix): ../ocgcore/ocgapi.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_ocgapi$(PreprocessSuffix) "E:/ygo/ocgcore/ocgapi.cpp"

$(IntermediateDirectory)/ocgcore_operations$(ObjectSuffix): ../ocgcore/operations.cpp $(IntermediateDirectory)/ocgcore_operations$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/operations.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_operations$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_operations$(DependSuffix): ../ocgcore/operations.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_operations$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_operations$(DependSuffix) -MM "E:/ygo/ocgcore/operations.cpp"

$(IntermediateDirectory)/ocgcore_operations$(PreprocessSuffix): ../ocgcore/operations.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_operations$(PreprocessSuffix) "E:/ygo/ocgcore/operations.cpp"

$(IntermediateDirectory)/ocgcore_playerop$(ObjectSuffix): ../ocgcore/playerop.cpp $(IntermediateDirectory)/ocgcore_playerop$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/playerop.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_playerop$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_playerop$(DependSuffix): ../ocgcore/playerop.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_playerop$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_playerop$(DependSuffix) -MM "E:/ygo/ocgcore/playerop.cpp"

$(IntermediateDirectory)/ocgcore_playerop$(PreprocessSuffix): ../ocgcore/playerop.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_playerop$(PreprocessSuffix) "E:/ygo/ocgcore/playerop.cpp"

$(IntermediateDirectory)/ocgcore_processor$(ObjectSuffix): ../ocgcore/processor.cpp $(IntermediateDirectory)/ocgcore_processor$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/processor.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_processor$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_processor$(DependSuffix): ../ocgcore/processor.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_processor$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_processor$(DependSuffix) -MM "E:/ygo/ocgcore/processor.cpp"

$(IntermediateDirectory)/ocgcore_processor$(PreprocessSuffix): ../ocgcore/processor.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_processor$(PreprocessSuffix) "E:/ygo/ocgcore/processor.cpp"

$(IntermediateDirectory)/ocgcore_scriptlib$(ObjectSuffix): ../ocgcore/scriptlib.cpp $(IntermediateDirectory)/ocgcore_scriptlib$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/ocgcore/scriptlib.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/ocgcore_scriptlib$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ocgcore_scriptlib$(DependSuffix): ../ocgcore/scriptlib.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ocgcore_scriptlib$(ObjectSuffix) -MF$(IntermediateDirectory)/ocgcore_scriptlib$(DependSuffix) -MM "E:/ygo/ocgcore/scriptlib.cpp"

$(IntermediateDirectory)/ocgcore_scriptlib$(PreprocessSuffix): ../ocgcore/scriptlib.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ocgcore_scriptlib$(PreprocessSuffix) "E:/ygo/ocgcore/scriptlib.cpp"

$(IntermediateDirectory)/client_card$(ObjectSuffix): client_card.cpp $(IntermediateDirectory)/client_card$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/client_card.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/client_card$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/client_card$(DependSuffix): client_card.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/client_card$(ObjectSuffix) -MF$(IntermediateDirectory)/client_card$(DependSuffix) -MM "E:/ygo/gframe/client_card.cpp"

$(IntermediateDirectory)/client_card$(PreprocessSuffix): client_card.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/client_card$(PreprocessSuffix) "E:/ygo/gframe/client_card.cpp"

$(IntermediateDirectory)/client_field$(ObjectSuffix): client_field.cpp $(IntermediateDirectory)/client_field$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/client_field.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/client_field$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/client_field$(DependSuffix): client_field.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/client_field$(ObjectSuffix) -MF$(IntermediateDirectory)/client_field$(DependSuffix) -MM "E:/ygo/gframe/client_field.cpp"

$(IntermediateDirectory)/client_field$(PreprocessSuffix): client_field.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/client_field$(PreprocessSuffix) "E:/ygo/gframe/client_field.cpp"

$(IntermediateDirectory)/deck_con$(ObjectSuffix): deck_con.cpp $(IntermediateDirectory)/deck_con$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/deck_con.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/deck_con$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/deck_con$(DependSuffix): deck_con.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/deck_con$(ObjectSuffix) -MF$(IntermediateDirectory)/deck_con$(DependSuffix) -MM "E:/ygo/gframe/deck_con.cpp"

$(IntermediateDirectory)/deck_con$(PreprocessSuffix): deck_con.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/deck_con$(PreprocessSuffix) "E:/ygo/gframe/deck_con.cpp"

$(IntermediateDirectory)/replay$(ObjectSuffix): replay.cpp $(IntermediateDirectory)/replay$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/replay.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/replay$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/replay$(DependSuffix): replay.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/replay$(ObjectSuffix) -MF$(IntermediateDirectory)/replay$(DependSuffix) -MM "E:/ygo/gframe/replay.cpp"

$(IntermediateDirectory)/replay$(PreprocessSuffix): replay.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/replay$(PreprocessSuffix) "E:/ygo/gframe/replay.cpp"

$(IntermediateDirectory)/lzma_Alloc$(ObjectSuffix): lzma/Alloc.c $(IntermediateDirectory)/lzma_Alloc$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "E:/ygo/gframe/lzma/Alloc.c" $(C_CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/lzma_Alloc$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lzma_Alloc$(DependSuffix): lzma/Alloc.c
	@$(C_CompilerName) $(C_CmpOptions) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lzma_Alloc$(ObjectSuffix) -MF$(IntermediateDirectory)/lzma_Alloc$(DependSuffix) -MM "E:/ygo/gframe/lzma/Alloc.c"

$(IntermediateDirectory)/lzma_Alloc$(PreprocessSuffix): lzma/Alloc.c
	@$(C_CompilerName) $(C_CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lzma_Alloc$(PreprocessSuffix) "E:/ygo/gframe/lzma/Alloc.c"

$(IntermediateDirectory)/lzma_LzFind$(ObjectSuffix): lzma/LzFind.c $(IntermediateDirectory)/lzma_LzFind$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "E:/ygo/gframe/lzma/LzFind.c" $(C_CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/lzma_LzFind$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lzma_LzFind$(DependSuffix): lzma/LzFind.c
	@$(C_CompilerName) $(C_CmpOptions) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lzma_LzFind$(ObjectSuffix) -MF$(IntermediateDirectory)/lzma_LzFind$(DependSuffix) -MM "E:/ygo/gframe/lzma/LzFind.c"

$(IntermediateDirectory)/lzma_LzFind$(PreprocessSuffix): lzma/LzFind.c
	@$(C_CompilerName) $(C_CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lzma_LzFind$(PreprocessSuffix) "E:/ygo/gframe/lzma/LzFind.c"

$(IntermediateDirectory)/lzma_LzmaDec$(ObjectSuffix): lzma/LzmaDec.c $(IntermediateDirectory)/lzma_LzmaDec$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "E:/ygo/gframe/lzma/LzmaDec.c" $(C_CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/lzma_LzmaDec$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lzma_LzmaDec$(DependSuffix): lzma/LzmaDec.c
	@$(C_CompilerName) $(C_CmpOptions) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lzma_LzmaDec$(ObjectSuffix) -MF$(IntermediateDirectory)/lzma_LzmaDec$(DependSuffix) -MM "E:/ygo/gframe/lzma/LzmaDec.c"

$(IntermediateDirectory)/lzma_LzmaDec$(PreprocessSuffix): lzma/LzmaDec.c
	@$(C_CompilerName) $(C_CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lzma_LzmaDec$(PreprocessSuffix) "E:/ygo/gframe/lzma/LzmaDec.c"

$(IntermediateDirectory)/lzma_LzmaEnc$(ObjectSuffix): lzma/LzmaEnc.c $(IntermediateDirectory)/lzma_LzmaEnc$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "E:/ygo/gframe/lzma/LzmaEnc.c" $(C_CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/lzma_LzmaEnc$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lzma_LzmaEnc$(DependSuffix): lzma/LzmaEnc.c
	@$(C_CompilerName) $(C_CmpOptions) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lzma_LzmaEnc$(ObjectSuffix) -MF$(IntermediateDirectory)/lzma_LzmaEnc$(DependSuffix) -MM "E:/ygo/gframe/lzma/LzmaEnc.c"

$(IntermediateDirectory)/lzma_LzmaEnc$(PreprocessSuffix): lzma/LzmaEnc.c
	@$(C_CompilerName) $(C_CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lzma_LzmaEnc$(PreprocessSuffix) "E:/ygo/gframe/lzma/LzmaEnc.c"

$(IntermediateDirectory)/lzma_LzmaLib$(ObjectSuffix): lzma/LzmaLib.c $(IntermediateDirectory)/lzma_LzmaLib$(DependSuffix)
	$(C_CompilerName) $(SourceSwitch) "E:/ygo/gframe/lzma/LzmaLib.c" $(C_CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/lzma_LzmaLib$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lzma_LzmaLib$(DependSuffix): lzma/LzmaLib.c
	@$(C_CompilerName) $(C_CmpOptions) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lzma_LzmaLib$(ObjectSuffix) -MF$(IntermediateDirectory)/lzma_LzmaLib$(DependSuffix) -MM "E:/ygo/gframe/lzma/LzmaLib.c"

$(IntermediateDirectory)/lzma_LzmaLib$(PreprocessSuffix): lzma/LzmaLib.c
	@$(C_CompilerName) $(C_CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lzma_LzmaLib$(PreprocessSuffix) "E:/ygo/gframe/lzma/LzmaLib.c"

$(IntermediateDirectory)/CIrrDeviceWin32$(ObjectSuffix): CIrrDeviceWin32.cpp $(IntermediateDirectory)/CIrrDeviceWin32$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "E:/ygo/gframe/CIrrDeviceWin32.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/CIrrDeviceWin32$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CIrrDeviceWin32$(DependSuffix): CIrrDeviceWin32.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CIrrDeviceWin32$(ObjectSuffix) -MF$(IntermediateDirectory)/CIrrDeviceWin32$(DependSuffix) -MM "E:/ygo/gframe/CIrrDeviceWin32.cpp"

$(IntermediateDirectory)/CIrrDeviceWin32$(PreprocessSuffix): CIrrDeviceWin32.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CIrrDeviceWin32$(PreprocessSuffix) "E:/ygo/gframe/CIrrDeviceWin32.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/gframe$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/gframe$(DependSuffix)
	$(RM) $(IntermediateDirectory)/gframe$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/game$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/game$(DependSuffix)
	$(RM) $(IntermediateDirectory)/game$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/data_manager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/data_manager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/data_manager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/image_manager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/image_manager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/image_manager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/CGUITTFont$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/CGUITTFont$(DependSuffix)
	$(RM) $(IntermediateDirectory)/CGUITTFont$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/event_handler$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/event_handler$(DependSuffix)
	$(RM) $(IntermediateDirectory)/event_handler$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/CGUIImageButton$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/CGUIImageButton$(DependSuffix)
	$(RM) $(IntermediateDirectory)/CGUIImageButton$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/drawing$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/drawing$(DependSuffix)
	$(RM) $(IntermediateDirectory)/drawing$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/network$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/network$(DependSuffix)
	$(RM) $(IntermediateDirectory)/network$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/deck_manager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/deck_manager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/deck_manager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/message$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/message$(DependSuffix)
	$(RM) $(IntermediateDirectory)/message$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_card$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_card$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_card$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_duel$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_duel$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_duel$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_effect$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_effect$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_effect$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_field$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_field$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_field$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_group$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_group$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_group$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_interpreter$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_interpreter$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_interpreter$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libcard$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libcard$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libcard$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libduel$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libduel$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libduel$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libeffect$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libeffect$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libeffect$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libgroup$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libgroup$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_libgroup$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_mem$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_mem$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_mem$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_ocgapi$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_ocgapi$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_ocgapi$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_operations$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_operations$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_operations$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_playerop$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_playerop$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_playerop$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_processor$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_processor$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_processor$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_scriptlib$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_scriptlib$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ocgcore_scriptlib$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/client_card$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/client_card$(DependSuffix)
	$(RM) $(IntermediateDirectory)/client_card$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/client_field$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/client_field$(DependSuffix)
	$(RM) $(IntermediateDirectory)/client_field$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/deck_con$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/deck_con$(DependSuffix)
	$(RM) $(IntermediateDirectory)/deck_con$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/replay$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/replay$(DependSuffix)
	$(RM) $(IntermediateDirectory)/replay$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/lzma_Alloc$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/lzma_Alloc$(DependSuffix)
	$(RM) $(IntermediateDirectory)/lzma_Alloc$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzFind$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzFind$(DependSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzFind$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzmaDec$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzmaDec$(DependSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzmaDec$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzmaEnc$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzmaEnc$(DependSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzmaEnc$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzmaLib$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzmaLib$(DependSuffix)
	$(RM) $(IntermediateDirectory)/lzma_LzmaLib$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/CIrrDeviceWin32$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/CIrrDeviceWin32$(DependSuffix)
	$(RM) $(IntermediateDirectory)/CIrrDeviceWin32$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile).exe
	$(RM) "E:\ygo\.build-release\gframe"


