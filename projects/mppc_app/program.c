#include <ConditionalMacros.h>
#include <MacTypes.h>
#include <NullDef.h>
#include <SizeTDef.h>
#include <VaListTDef.h>
#include <StdArg.h>
#include <WCharTDef.h>
#include <StdDef.h>
#include <SeekDefs.h>
#include <StdIO.h>
#include <StdLib.h>
#include <Limits.h>
#include <CType.h>
#include <CFBase.h>
#include <CFArray.h>
#include <CFData.h>
#include <CFDictionary.h>
#include <CFString.h>
#include <MacErrors.h>
#include <MixedMode.h>
#include <MacMemory.h>
#include <Patches.h>
#include <DateTimeUtils.h>
#include <OSUtils.h>
#include <TextCommon.h>
#include <UTCUtils.h>
#include <Finder.h>
#include <Files.h>
#include <CFURL.h>
#include <CFBundle.h>
#include <CGBase.h>
#include <CGGeometry.h>
#include <CGAffineTransform.h>
#include <CGDataProvider.h>
#include <CGColorSpace.h>
#include <CGFont.h>
#include <CGImage.h>
#include <CGPDFDocument.h>
#include <CGContext.h>
#include <Components.h>
#include <IntlResources.h>
#include <QuickdrawText.h>
#include <Quickdraw.h>
#include <NameRegistry.h>
#include <Multiprocessing.h>
#include <CodeFragments.h>
#include <DriverFamilyMatching.h>
#include <Disks.h>
#include <Devices.h>
#include <Aliases.h>
#include <NumberFormatting.h>

#include <SegLoad.h>
#include <Resources.h>
#include <Events.h>
#include <AEDataModel.h>
#include <Notification.h>
#include <AEInteraction.h>
#include <AppleEvents.h>
#include <Drag.h>
#include <TextEdit.h>
#include <AERegistry.h>
#include <ATSTypes.h>
#include <Fonts.h>
#include <Processes.h>
#include <Collections.h>
#include <CFUUID.h>
#include <Menus.h>
#include <QDOffscreen.h>
#include <TextCommon.h>
#include <Icons.h>
#include <MacWindows.h>
#include <Controls.h>
#include <CarbonEvents.h>
#include <Dialogs.h>
#include <ControlDefinitions.h>
#include <Traps.h>
#include <Scrap.h>
#include <ToolUtils.h>
#include <DiskInit.h>

#define ShouldDefineQDGlobals 1

#if ShouldDefineQDGlobals
/* GLOBALVAR */ QDGlobals qd;
#endif

#define useUPP 1

#define IsAnApp 1

#include <app.c>
