// -*- c-basic-offset: 4 -*-

/*
    Rosegarden-4
    A sequencer and musical notation editor.

    This program is Copyright 2000-2006
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <bownie@bownie.com>

    The moral right of the authors to claim authorship of this work
    has been asserted.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef ROSEGARDENGUI_H
#define ROSEGARDENGUI_H

#include <qstrlist.h>
#include <qcursor.h>

#include <kapp.h>
#include <kdockwidget.h>
#include <kaccel.h>

#include "tempodialog.h"
#include "rosegardendcop.h"
#include "rosegardenguiiface.h"
#include "rosegardenparameterarea.h"

#include "AudioFile.h"
#include "Sound.h"
#include "MappedEvent.h"
#include "MappedCommon.h"
#include "Device.h"

class KURL;
class KRecentFilesAction;
class KToggleAction;
class KProcess;
class QTextCodec;

// forward declaration of the RosegardenGUI classes
class BankEditorDialog;
class DeviceManagerDialog;
class SynthPluginManagerDialog;
class AudioMixerWindow;
class MidiMixerWindow;
class RosegardenGUIDoc;
class RosegardenGUIView;
template <class T> class ZoomSlider;

namespace Rosegarden
{
    class AudioManagerDialog;
    class AudioPluginManager;
    class AudioPluginDialog;
    class RosegardenTransportDialog;
    class SequenceManager;
    class Clipboard;
    class SegmentSelection;
}

class RosegardenProgressBar;
class ControlEditorDialog;
class MarkerEditorDialog;
class TempoView;
class TriggerSegmentManager;
class PlayListDialog;
class SegmentParameterBox;
class InstrumentParameterBox;
class TrackParameterBox;

class StartupTester;

#ifdef HAVE_LIBLO
class AudioPluginOSCGUIManager;
#endif

#ifdef HAVE_LIRC
class LircClient;
class LircCommander;
#endif

/**
  * The base class for RosegardenGUI application windows. It sets up the main
  * window and reads the config file as well as providing a menubar, toolbar
  * and statusbar. An instance of RosegardenGUIView creates your center view, which is connected
  * to the window's Doc object.
  * RosegardenGUIApp reimplements the methods that KTMainWindow provides for main window handling and supports
  * full session management as well as keyboard accelerator configuration by using KAccel.
  * @see KTMainWindow
  * @see KApplication
  * @see KConfig
  * @see KAccel
  *
  * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
  * @version KDevelop version 0.4 code generation
  */
class RosegardenGUIApp : public KDockMainWindow, virtual public RosegardenIface
{
  Q_OBJECT

  friend class RosegardenGUIView;

public:

    /**
     * construtor of RosegardenGUIApp, calls all init functions to
     * create the application.
     * \arg useSequencer : if true, the sequencer is launched
     * @see initMenuBar initToolBar
     */
    RosegardenGUIApp(bool useSequencer = true,
		     bool useExistingSequencer = false,
                     QObject *startupStatusMessageReceiver = 0);

    virtual ~RosegardenGUIApp();

    /*
     * Get the current copy of the app object
     */
    static RosegardenGUIApp *self() { return m_myself; }
    
    /**
     * returns a pointer to the current document connected to the
     * KTMainWindow instance and is used by * the View class to access
     * the document object's methods
     */ 
    RosegardenGUIDoc *getDocument() const;      

    RosegardenGUIView* getView() { return m_view; }

    Rosegarden::RosegardenTransportDialog* getTransport();

    enum ImportType { ImportRG4, ImportMIDI, ImportRG21, ImportHydrogen, ImportCheckType };

    /**
     * open a Rosegarden file
     */
    virtual void openFile(QString filePath) { openFile(filePath, ImportCheckType); }

    /**
     * open a file, explicitly specifying its type
     */
    void openFile(QString filePath, ImportType type);

    /**
     * decode and open a project file
     */
    void importProject(QString filePath);

    /**
     * open a URL
     */
    virtual void openURL(QString url);

    /**
     * merge a file with the existing document
     */ 
    virtual void mergeFile(QString filePath) { mergeFile(filePath, ImportCheckType); }
    
    /**
     * merge a file, explicitly specifying its type
     */
    void mergeFile(QString filePath, ImportType type);

    /**
     * open a URL
     */
    void openURL(const KURL &url);

    /**
     * export a MIDI file
     */
    void exportMIDIFile(QString url);

    /**
     * export a Csound scorefile
     */
    void exportCsoundFile(QString url);

    /**
     * export a Mup file
     */
    void exportMupFile(QString url);

    /**
     * export a Lilypond file
     */
    bool exportLilypondFile(QString url, bool forPreview = false);

    /**
     * export a MusicXml file
     */
    void exportMusicXmlFile(QString url);

    /**
     * Get the sequence manager object
     */
    Rosegarden::SequenceManager* getSequenceManager() { return m_seqManager; }

    /**
     * Get a progress bar
     */
    RosegardenProgressBar *getProgressBar() { return m_progressBar; }

    /**
     * Equivalents of the GUI slots, for DCOP use
     */
    virtual void fileNew()    { slotFileNew(); }
    virtual void fileSave()   { slotFileSave(); }
    virtual void fileClose()  { slotFileClose(); }
    virtual void quit()       { slotQuit(); }

    virtual void play()               { slotPlay(); }
    virtual void stop()               { slotStop(); }
    virtual void rewind()             { slotRewind(); }
    virtual void fastForward()        { slotFastforward(); }
    virtual void record()             { slotRecord(); }
    virtual void rewindToBeginning()  { slotRewindToBeginning(); }
    virtual void fastForwardToEnd()   { slotFastForwardToEnd(); }
    virtual void jumpToTime(int sec, int usec) { slotJumpToTime(sec, usec); }
    virtual void startAtTime(int sec, int usec) { slotStartAtTime(sec, usec); }

    /**
     * Start the sequencer auxiliary process
     * (built in the 'sequencer' directory)
     *
     * @see slotSequencerExited()
     */
    bool launchSequencer(bool useExistingSequencer);

#ifdef HAVE_LIBJACK
    /*
     * Launch and control JACK if required to by configuration 
     */
    bool launchJack();

#endif // HAVE_LIBJACK


    /**
     * Returns whether we're using a sequencer.
     * false if the '--nosequencer' option was given
     * true otherwise.
     * This doesn't give the state of the sequencer
     * @see #isSequencerRunning
     */
    bool isUsingSequencer() { return m_useSequencer; }

    /**
     * Returns whether there's a sequencer running.
     * The result is dynamically updated depending on the sequencer's
     * status.
     */
    bool isSequencerRunning() { return m_useSequencer && (m_sequencerProcess != 0); }

    /**
     * Returns true if the sequencer wasn't started by us
     */
    bool isSequencerExternal() { return m_useSequencer && (m_sequencerProcess == SequencerExternal); }
    
    /**
     * Set the sequencer status - pass through DCOP as an int
     */
    virtual void notifySequencerStatus(int status);

    /**
     * Handle some random incoming MIDI events.
     */
    virtual void processAsynchronousMidi(const Rosegarden::MappedComposition &);

    /*
     * The sequencer calls this method when it's running to
     * allow us to sync data with it.
     *
     */
    virtual void alive();

    /*
     * Return the clipboard
     */
    Rosegarden::Clipboard* getClipboard() { return m_clipboard; }

#ifdef HAVE_LIBLO
    /**
     * Return the plugin native GUI manager, if we have one
     */
    AudioPluginOSCGUIManager *getPluginGUIManager() { return m_pluginGUIManager; }
#endif

    /**
     * Plug a widget into our common accelerators
     */
    void plugAccelerators(QWidget *widget, QAccel *accel);

    /**
     * Override from QWidget
     * Toolbars and docks need special treatment
     */
    virtual void setCursor(const QCursor&);

protected:

    /**** File handling code that we don't want the outside world to use ****/
    /**/
    /**/

    /**
     * Create document from a file
     */
    RosegardenGUIDoc* createDocument(QString filePath, ImportType type = ImportRG4);

    /**
     * Create a document from RG file
     */
    RosegardenGUIDoc* createDocumentFromRGFile(QString filePath);

    /**
     * Create document from MIDI file
     */
    RosegardenGUIDoc* createDocumentFromMIDIFile(QString filePath);

    /**
     * Create document from RG21 file
     */
    RosegardenGUIDoc* createDocumentFromRG21File(QString filePath);

    /**
     * Create document from Hydrogen drum machine file
     */
    RosegardenGUIDoc* createDocumentFromHydrogenFile(QString filePath);

    /**/
    /**/
    /***********************************************************************/

    /**
     * Set the 'Rewind' and 'Fast Forward' buttons in the transport
     * toolbar to AutoRepeat
     */
    void setRewFFwdToAutoRepeat();

    static const void* SequencerExternal;

    /// Raise the transport along
    virtual void showEvent(QShowEvent*);

    /**
     * read general Options again and initialize all variables like
     * the recent file list
     */
    void readOptions();

    /**
     * add an item pointing to the example files in the KFileDialog speedbar
     */
    void setupFileDialogSpeedbar();

    /**
     * create menus and toolbars
     */
    void setupActions();

    /**
     * sets up the zoom toolbar
     */
    void initZoomToolbar();

    /**
     * sets up the statusbar for the main window by initialzing a
     * statuslabel.
     */
    void initStatusBar();

    /**
     * creates the centerwidget of the KTMainWindow instance and sets
     * it as the view
     */
    void initView();

    /**
     * queryClose is called by KTMainWindow on each closeEvent of a
     * window. Against the default implementation (only returns true),
     * this calls saveModified() on the document object to ask if the
     * document shall be saved if Modified; on cancel the closeEvent
     * is rejected.
     *
     * @see KTMainWindow#queryClose
     * @see KTMainWindow#closeEvent
     */
    virtual bool queryClose();

    /**
     * queryExit is called by KTMainWindow when the last window of the
     * application is going to be closed during the closeEvent().
     * Against the default implementation that just returns true, this
     * calls saveOptions() to save the settings of the last window's
     * properties.
     *
     * @see KTMainWindow#queryExit
     * @see KTMainWindow#closeEvent
     */
    virtual bool queryExit();

    /**
     * saves the window properties for each open window during session
     * end to the session config file, including saving the currently
     * opened file by a temporary filename provided by KApplication.
     *
     * @see KTMainWindow#saveProperties
     */
    virtual void saveGlobalProperties(KConfig *_cfg);

    /**
     * reads the session config file and restores the application's
     * state including the last opened files and documents by reading
     * the temporary files saved by saveProperties()
     *
     * @see KTMainWindow#readProperties
     */
    virtual void readGlobalProperties(KConfig *_cfg);

    /**
     * Create a new audio file for the sequencer and return the
     * path to it as a QString.
     */
    QString createNewAudioFile();
    QValueVector<QString> createRecordAudioFiles(const QValueVector<Rosegarden::InstrumentId> &);

    QString getAudioFilePath();

    //!!!mtr
    QValueVector<Rosegarden::InstrumentId> getArmedInstruments();

    /**
     * Show a sequencer error to the user.  This is for errors from
     * the framework code; the playback code uses mapped compositions
     * to send these things back asynchronously.
     */
    void showError(QString error);

    /*
     * Return AudioManagerDialog
     */
    Rosegarden::AudioManagerDialog* getAudioManagerDialog() { return m_audioManagerDialog; }

    /**
     * Ask the user for a file to save to, and check that it's
     * good and that (if it exists) the user agrees to overwrite.
     * Return a null string if the write should not go ahead.
     */
    QString getValidWriteFile(QString extension, QString label);

    /**
     * Find any non-ASCII strings in a composition that has been
     * generated by MIDI import or any other procedure that produces
     * events with unknown text encoding, and ask the user what
     * encoding to translate them from.  This assumes all text strings
     * in the composition are of the same encoding, and that it is not
     * (known to be) utf8 (in which case no transcoding would be
     * necessary).
     */
    void fixTextEncodings(Rosegarden::Composition *);
    QTextCodec *guessTextCodec(std::string);

    /**
     * Set the current document
     *
     * Do all the needed housework when the current document changes
     * (like closing edit views, emitting documentChanged signal, etc...)
     */
    void setDocument(RosegardenGUIDoc*);

    /**
     * Jog a selection of segments by an amount
     */
    void jogSelection(Rosegarden::timeT amount);

    void createAndSetupTransport();

signals:
    void startupStatusMessage(QString message);

    /// emitted just before the document is changed
    void documentAboutToChange();

    /// emitted when the current document changes
    void documentChanged(RosegardenGUIDoc*);

    /// emitted when the set of selected segments changes (relayed from RosegardenGUIView)
    void segmentsSelected(const Rosegarden::SegmentSelection &);

    /// emitted when the composition state (selected track, solo, etc...) changes
    void compositionStateUpdate();

    /// emitted when instrument parameters change (relayed from InstrumentParameterBox)
    void instrumentParametersChanged(Rosegarden::InstrumentId);

    /// emitted when a plugin dialog selects a plugin
    void pluginSelected(Rosegarden::InstrumentId, int, int);

    /// emitted when a plugin dialog (un)bypasses a plugin
    void pluginBypassed(Rosegarden::InstrumentId, int, bool);

public slots:

    /**
     * open a URL - used for Dn'D
     *
     * @param url : a string containing a url (protocol://foo/bar/file.rg)
     */
    virtual void slotOpenDroppedURL(QString url);

    /**
     * Open the document properties dialog on the Audio page
     */
    virtual void slotOpenAudioPathSettings();

    /**
     * open a new application window by creating a new instance of
     * RosegardenGUIApp
     */
    void slotFileNewWindow();

    /**
     * clears the document in the actual view to reuse it as the new
     * document
     */
    void slotFileNew();

    /**
     * open a file and load it into the document
     */
    void slotFileOpen();

    /**
     * opens a file from the recent files menu
     */
    void slotFileOpenRecent(const KURL&);

    /**
     * save a document
     */
    void slotFileSave();

    /**
     * save a document by a new filename
     */
    bool slotFileSaveAs();

    /**
     * asks for saving if the file is modified, then closes the actual
     * file and window
     */
    void slotFileClose();

    /**
     * print the actual file
     */
    void slotFilePrint();

    /**
     * print preview 
     */
    void slotFilePrintPreview();

    /**
     * Let the user select a Rosegarden Project file for import
     */
    void slotImportProject();

    /**
     * Let the user select a MIDI file for import
     */
    void slotImportMIDI();

    /**
     * Revert to last loaded file
     */
    void slotRevertToSaved();

    /**
     * Let the user select a Rosegarden 2.1 file for import 
     */
    void slotImportRG21();

    /**
     * Select a Hydrogen drum machine file for import
     */
    void slotImportHydrogen();

    /**
     * Let the user select a MIDI file for merge
     */
    void slotMerge();

    /**
     * Let the user select a MIDI file for merge
     */
    void slotMergeMIDI();

    /**
     * Let the user select a MIDI file for merge
     */
    void slotMergeRG21();

    /**
     * Select a Hydrogen drum machine file for merge
     */
    void slotMergeHydrogen();

    /**
     * Let the user export a Rosegarden Project file
     */
    void slotExportProject();

    /**
     * Let the user enter a MIDI file to export to
     */
    void slotExportMIDI();

    /**
     * Let the user enter a Csound scorefile to export to
     */
    void slotExportCsound();

    /**
     * Let the user enter a Mup file to export to
     */
    void slotExportMup();

    /**
     * Let the user enter a Lilypond file to export to
     */
    void slotExportLilypond();

    /**
     * Export to a temporary file and process
     */
    void slotPreviewLilypond();
    void slotLilypondViewProcessExited(KProcess *);

    /**
     * Let the user enter a MusicXml file to export to
     */
    void slotExportMusicXml();

    /**
     * closes all open windows by calling close() on each memberList
     * item until the list is empty, then quits the application.  If
     * queryClose() returns false because the user canceled the
     * saveModified() dialog, the closing breaks.
     */
    void slotQuit();
    
    /**
     * put the marked text/object into the clipboard and remove * it
     * from the document
     */
    void slotEditCut();

    /**
     * put the marked text/object into the clipboard
     */
    void slotEditCopy();

    /**
     * paste the clipboard into the document
     */
    void slotEditPaste();
    
    /**
     * Cut a time range (sections of segments, tempo, and time
     * signature events within that range).
     */
    void slotCutRange();

    /**
     * Copy a time range.
     */
    void slotCopyRange();

    /**
     * Paste the clipboard at the current pointer position, moving all
     * subsequent material along to make space.
     */
    void slotPasteRange();
    
    /**
     * Delete a time range.
     */
    void slotDeleteRange();

    /**
     * select all segments on all tracks
     */
    void slotSelectAll();

    /**
     * delete selected segments, duh
     */
    void slotDeleteSelectedSegments();

    /**
     * Quantize the selected segments (after asking the user how)
     */
    void slotQuantizeSelection();

    /**
     * Quantize the selected segments by repeating the last iterative quantize
     */
    void slotRepeatQuantizeSelection();

    /**
     * Calculate timing/tempo info based on selected segment
     */
    void slotGrooveQuantize();

    /**
     * Rescale the selected segments by a factor requested from
     * the user
     */
    void slotRescaleSelection();

    /**
     * Split the selected segments on silences (or new timesig, etc)
     */
    void slotAutoSplitSelection();

    /**
     * Jog a selection left or right by an amount
     */
    void slotJogRight();
    void slotJogLeft();

    /**
     * Split the selected segments by pitch
     */
    void slotSplitSelectionByPitch();

    /**
     * Split the selected segments by recorded source
     */
    void slotSplitSelectionByRecordedSrc();

    /**
     * Split the selected segments at some time
     */
    void slotSplitSelectionAtTime();

    /**
     * Produce a harmony segment from the selected segments
     */
    void slotHarmonizeSelection();

    /**
     * Set the start times of the selected segments
     */
    void slotSetSegmentStartTimes();

    /**
     * Set the durations of the selected segments
     */
    void slotSetSegmentDurations();

    /**
     * Merge the selected segments
     */
    void slotJoinSegments();

    /**
     * Tempo to Segment length
     */
    void slotTempoToSegmentLength();
    void slotTempoToSegmentLength(QWidget* parent);

    /**
     * toggle segment labels
     */
    void slotToggleSegmentLabels();
    
    /**
     * open the default editor for each of the currently-selected segments
     */
    void slotEdit();

    /**
     * open an event list view for each of the currently-selected segments
     */
    void slotEditInEventList();

    /**
     * open a matrix view for each of the currently-selected segments
     */
    void slotEditInMatrix();

    /**
     * open a percussion matrix view for each of the currently-selected segments
     */
    void slotEditInPercussionMatrix();

    /**
     * open a notation view with all currently-selected segments in it
     */
    void slotEditAsNotation();

    /**
     * open a tempo/timesig edit view
     */
    void slotEditTempos();
    void slotEditTempos(Rosegarden::timeT openAtTime);

    /**
     * Edit the tempo - called from a Transport signal
     */
    void slotEditTempo();
    void slotEditTempo(Rosegarden::timeT atTime);
    void slotEditTempo(QWidget *parent);
    void slotEditTempo(QWidget *parent, Rosegarden::timeT atTime);

    /**
     * Edit the time signature - called from a Transport signal
     */
    void slotEditTimeSignature();
    void slotEditTimeSignature(Rosegarden::timeT atTime);
    void slotEditTimeSignature(QWidget *parent);
    void slotEditTimeSignature(QWidget *parent, Rosegarden::timeT atTime);

    /**
     * Change the length of the composition
     */
    void slotChangeCompositionLength();

    /**
     * open a dialog for document properties
     */
    void slotEditDocumentProperties();

    /**
     * Manage MIDI Devices
     */
    void slotManageMIDIDevices();

    /**
     * Manage plugin synths
     */
    void slotManageSynths();

    /**
     * Show the mixers
     */
    void slotOpenAudioMixer();
    void slotOpenMidiMixer();
    
    /**
     * Edit Banks/Programs
     */
    void slotEditBanks();

    /**
     * Edit Banks/Programs for a particular device
     */
    void slotEditBanks(Rosegarden::DeviceId);

    /**
     * Edit Control Parameters for a particular device
     */
    void slotEditControlParameters(Rosegarden::DeviceId);

    /**
     * Edit Document Markers
     */
    void slotEditMarkers();

    /**
     * Not an actual action slot : populates the set_track_instrument sub menu
     */
    void slotPopulateTrackInstrumentPopup();

    /**
     * Remap instruments
     */
    void slotRemapInstruments();

    /**
     * Modify MIDI filters
     */
    void slotModifyMIDIFilters();

    /**
     * Manage Metronome
     */
    void slotManageMetronome();

    /**
     * Save Studio as Default
     */
    void slotSaveDefaultStudio();

    /**
     * Import Studio from File
     */
    void slotImportStudio();

    /**
     * Import Studio from Autoload
     */
    void slotImportDefaultStudio();

    /**
     * Import Studio from File
     */
    void slotImportStudioFromFile(const QString &file);

    /**
     * Send MIDI_RESET to all MIDI devices
     */
    void slotResetMidiNetwork();
    
    /**
     * toggles the toolbar
     */
    void slotToggleToolBar();

    /**
     * toggles the transport window
     */
    void slotToggleTransport();

    /**
     * toggles the tools toolbar
     */
    void slotToggleToolsToolBar();

    /**
     * toggles the tracks toolbar
     */
    void slotToggleTracksToolBar();

    /**
     * toggles the editors toolbar
     */
    void slotToggleEditorsToolBar();

    /**
     * toggles the transport toolbar
     */
    void slotToggleTransportToolBar();

    /**
     * toggles the zoom toolbar
     */
    void slotToggleZoomToolBar();

    /**
     * toggles the statusbar
     */
    void slotToggleStatusBar();

    /**
     * changes the statusbar contents for the standard label
     * permanently, used to indicate current actions.
     *
     * @param text the text that is displayed in the statusbar
     */
    void slotStatusMsg(QString text);

    /**
     * changes the status message of the whole statusbar for two
     * seconds, then restores the last status. This is used to display
     * statusbar messages that give information about actions for
     * toolbar icons and menuentries.
     *
     * @param text the text that is displayed in the statusbar
     */
    void slotStatusHelpMsg(QString text);

    /**
     * enables/disables the transport window
     */
    void slotEnableTransport(bool);

    /**
     * segment select tool
     */
    void slotPointerSelected();

    /**
     * segment eraser tool is selected
     */
    void slotEraseSelected();
    
    /**
     * segment draw tool is selected
     */
    void slotDrawSelected();
    
    /**
     * segment move tool is selected
     */
    void slotMoveSelected();

    /**
     * segment resize tool is selected
     */
    void slotResizeSelected();

    /*
     * Segment join tool
     *
     */
    void slotJoinSelected();

    /*
     * Segment split tool
     *
     */
    void slotSplitSelected();

    /**
     * Add one new track
     */
    void slotAddTrack();

    /**
     * Add new tracks
     */
    void slotAddTracks();

    /*
     * Delete Tracks
     */
    void slotDeleteTrack();

    /*
     * Modify track position
     */
    void slotMoveTrackUp();
    void slotMoveTrackDown();

    /**
     * timeT version of the same
     */
    void slotSetPointerPosition(Rosegarden::timeT t);

    /**
     * Set the pointer position and start playing (from LoopRuler)
     */
    void slotSetPlayPosition(Rosegarden::timeT position);

    /**
     * Set a loop
     */
    void slotSetLoop(Rosegarden::timeT lhs, Rosegarden::timeT rhs);


    /**
     * Update the transport with the bar, beat and unit times for
     * a given timeT
     */
    void slotDisplayBarTime(Rosegarden::timeT t);


    /**
     * Transport controls
     */
    void slotPlay();
    void slotStop();
    void slotRewind();
    void slotFastforward();
    void slotRecord();
    void slotToggleRecord();
    void slotRewindToBeginning();
    void slotFastForwardToEnd();
    void slotJumpToTime(int sec, int usec);
    void slotStartAtTime(int sec, int usec);
    void slotRefreshTimeDisplay();
    void slotToggleTracking();

    /**
     * Called when the sequencer auxiliary process exits
     */
    void slotSequencerExited(KProcess*);

    /// When the transport closes 
    void slotCloseTransport();

    /**
     * called by RosegardenApplication when session management tells
     * it to save its state. This is to avoid saving the transport as
     * a 2nd main window
     */
    void slotDeleteTransport();
    
    /**
     * Put the GUI into a given Tool edit mode
     */
    void slotActivateTool(QString toolName);

    /**
     * Toggles either the play or record metronome according
     * to Transport status
     */
    void slotToggleMetronome();

    /*
     * Toggle the solo mode
     */
    void slotToggleSolo(bool);

    /**
     * Set and unset the loop from the transport loop button with
     * these slots.
     */
    void slotSetLoop();
    void slotUnsetLoop();

    /**
     * Set and unset the loop start/end time from the transport loop start/stop buttons with
     * these slots.
     */
    void slotSetLoopStart();
    void slotSetLoopStop();

    /**
     * Toggle the track labels on the TrackEditor
     */
    void slotToggleTrackLabels();

    /**
     * Toggle the rulers on the TrackEditor
     * (aka bar buttons)
     */
    void slotToggleRulers();

    /**
     * Toggle the tempo ruler on the TrackEditor
     */
    void slotToggleTempoRuler();

    /**
     * Toggle the chord-name ruler on the TrackEditor
     */
    void slotToggleChordNameRuler();

    /**
     * Toggle the segment canvas previews
     */
    void slotTogglePreviews();

    /**
     * Re-dock the parameters box to its initial position
     */
    void slotDockParametersBack();

    /**
     * The parameters box was closed
     */
    void slotParametersClosed();

    /**
     * The parameters box was docked back
     */
    void slotParametersDockedBack(KDockWidget*, KDockWidget::DockPosition);

    /**
     * Display tip-of-day dialog on demand
     */
    void slotShowTip();

    /*
     * Select Track up or down
     */
    void slotTrackUp();
    void slotTrackDown();

    /**
     * Mute/Unmute
     */
    void slotMuteAllTracks();
    void slotUnmuteAllTracks();

    /**
     * save general Options like all bar positions and status as well
     * as the geometry and the recent file list to the configuration
     * file
     */         
    void slotSaveOptions();

    /**
     * Show the configure dialog
     */
    void slotConfigure();

    /**
     * Show the key mappings
     *
     */
    void slotEditKeys();

    /**
     * Edit toolbars
     */
    void slotEditToolbars();

    /**
     * Update the toolbars after edition
     */
    void slotUpdateToolbars();

    /**
     * Zoom slider moved
     */
    void slotChangeZoom(int index);

    void slotZoomIn();
    void slotZoomOut();

    /**
     * Modify tempo
     */
    void slotChangeTempo(Rosegarden::timeT time,
                         Rosegarden::tempoT value,      
                         Rosegarden::tempoT target,
                         TempoDialog::TempoDialogAction action);

    /**
     * Move a tempo change
     */
    void slotMoveTempo(Rosegarden::timeT oldTime,
		       Rosegarden::timeT newTime);

    /**
     * Remove a tempo change
     */
    void slotDeleteTempo(Rosegarden::timeT time);

    /**
     * Document modified
     */
    void slotDocumentModified(bool modified = true);


    /**
     * This slot is here to be connected to RosegardenGUIView's
     * stateChange signal. We use a bool for the 2nd arg rather than a
     * KXMLGUIClient::ReverseStateChange to spare the include of
     * kxmlguiclient.h just for one typedef.
     *
     * Hopefully we'll be able to get rid of this eventually,
     * I should slip this in KMainWindow for KDE 4.
     */
    void slotStateChanged(QString, bool noReverse);

    /**
     * A command has happened; check the clipboard in case we
     * need to change state
     */
    void slotTestClipboard();

    /**
     * Show a 'play list' dialog
     */
    void slotPlayList();

    /**
     * Play the requested URL
     *
     * Stop current playback, close current document,
     * open specified document and play it.
     */
    void slotPlayListPlay(QString url);

    /**
     * Call up the online tutorial
     */
    void slotTutorial();

    /**
     * Surf to the bug reporting guidelines
     */
    void slotBugGuidelines();
    
    /**
     * View the trigger segments manager
     */
    void slotManageTriggerSegments();

    /**
     * View the audio file manager - and some associated actions
     */
    void slotAudioManager();

    void slotAddAudioFile(Rosegarden::AudioFileId);
    void slotDeleteAudioFile(Rosegarden::AudioFileId);
    void slotPlayAudioFile(Rosegarden::AudioFileId,
                           const Rosegarden::RealTime &,
                           const Rosegarden::RealTime &);
    void slotCancelAudioPlayingFile(Rosegarden::AudioFileId);
    void slotDeleteAllAudioFiles();

    /**
     * Reflect segment deletion from the audio manager
     */
    void slotDeleteSegments(const Rosegarden::SegmentSelection&);

    void slotRepeatingSegments();
    void slotRelabelSegments();

    /// Panic button pressed
    void slotPanic();

    // Auto-save
    //
    void slotAutoSave();

    // Auto-save update interval changes
    //
    void slotUpdateAutoSaveInterval(unsigned int interval);

    // Update the side-bar when the configuration page changes its style.
    //
    void slotUpdateSidebarStyle(unsigned int style);

    /**
     * called when the PlayList is being closed
     */
    void slotPlayListClosed();

    /**
     * called when the BankEditor is being closed
     */
    void slotBankEditorClosed();

    /**
     * called when the Device Manager is being closed
     */
    void slotDeviceManagerClosed();

    /**
     * called when the synth manager is being closed
     */
    void slotSynthPluginManagerClosed();

    /**
     * called when the Mixer is being closed
     */
    void slotAudioMixerClosed();
    void slotMidiMixerClosed();

    /**
     * when ControlEditor is being closed
     */
    void slotControlEditorClosed();

    /**
     * when MarkerEditor is being closed
     */
    void slotMarkerEditorClosed();

    /**
     * when TempoView is being closed
     */
    void slotTempoViewClosed();

    /**
     * when TriggerManager is being closed
     */
    void slotTriggerManagerClosed();

    /**
     * when AudioManagerDialog is being closed
     */
    void slotAudioManagerClosed();

    /**
     * Update the pointer position from the sequencer mmapped file when playing
     */
    void slotUpdatePlaybackPosition();

    /**
     * Update the CPU level meter
     */
    void slotUpdateCPUMeter(bool playing);

    /**
     * Update the monitor levels from the sequencer mmapped file when not playing
     * (slotUpdatePlaybackPosition does this among other things when playing)
     */
    void slotUpdateMonitoring();

    /**
     * Create a plugin dialog for a given instrument and slot, or
     * raise an exising one.
     */
    void slotShowPluginDialog(QWidget *parent,
			      Rosegarden::InstrumentId instrument,
			      int index);

    void slotPluginSelected(Rosegarden::InstrumentId instrument,
			    int index, int plugin);

    /**
     * An external GUI has requested a port change.
     */
    void slotChangePluginPort(Rosegarden::InstrumentId instrument,
			      int index, int portIndex, float value);

    /**
     * Our internal GUI has made a port change -- the
     * PluginPortInstance already contains the new value, but we need
     * to inform the sequencer and update external GUIs.
     */
    void slotPluginPortChanged(Rosegarden::InstrumentId instrument,
			       int index, int portIndex);

    /**
     * An external GUI has requested a program change.
     */
    void slotChangePluginProgram(Rosegarden::InstrumentId instrument,
				 int index, QString program);

    /**
     * Our internal GUI has made a program change -- the
     * AudioPluginInstance already contains the new program, but we
     * need to inform the sequencer, update external GUIs, and update
     * the port values for the new program.
     */
    void slotPluginProgramChanged(Rosegarden::InstrumentId instrument,
				  int index);

    /**
     * An external GUI has requested a configure call.  (This can only
     * happen from an external GUI, we have no way to manage these
     * internally.)
     */
    void slotChangePluginConfiguration(Rosegarden::InstrumentId, int index,
				       bool global, QString key, QString value);
    void slotPluginDialogDestroyed(Rosegarden::InstrumentId instrument,
				   int index);
    void slotPluginBypassed(Rosegarden::InstrumentId,
			    int index, bool bypassed);

    void slotShowPluginGUI(Rosegarden::InstrumentId, int index);
    void slotStopPluginGUI(Rosegarden::InstrumentId, int index);
    void slotPluginGUIExited(Rosegarden::InstrumentId, int index);

    void slotDocumentDevicesResyncd();

    void slotTestStartupTester();
    
    void slotDebugDump();

    /**
     * Enable or disable the internal MIDI Thru routing. 
     * 
     * This policy is implemented at the sequencer side, controlled 
     * by this flag and also by the MIDI Thru filters.
     * 
     * @see ControlBlock::isMidiRoutingEnabled()
     * @see RosegardenSequencerApp::processAsynchronousEvents()
     * @see RosegardenSequencerApp::processRecordedEvents()
     */
    void slotEnableMIDIThruRouting();

private:


    //--------------- Data members ---------------------------------

    bool m_actionsSetup;

    KRecentFilesAction* m_fileRecent;

    /**
     * view is the main widget which represents your working area. The
     * View class should handle all events of the view widget.  It is
     * kept empty so you can create your view according to your
     * application's needs by changing the view class.
     */
    RosegardenGUIView* m_view;
    RosegardenGUIView* m_swapView;

    KDockWidget* m_mainDockWidget;
    KDockWidget* m_dockLeft;

    /**
     * doc represents your actual document and is created only
     * once. It keeps information such as filename and does the
     * serialization of your files.
     */
    RosegardenGUIDoc* m_doc;

    /**
     * KAction pointers to enable/disable actions
     */
    KRecentFilesAction* m_fileOpenRecent;

    KToggleAction* m_viewToolBar;
    KToggleAction* m_viewToolsToolBar;
    KToggleAction* m_viewTracksToolBar;
    KToggleAction* m_viewEditorsToolBar;
    KToggleAction* m_viewZoomToolBar;
    KToggleAction* m_viewStatusBar;
    KToggleAction* m_viewTransport;
    KToggleAction* m_viewTransportToolBar;
    KToggleAction* m_viewTrackLabels;
    KToggleAction* m_viewRulers;
    KToggleAction* m_viewTempoRuler;
    KToggleAction* m_viewChordNameRuler;
    KToggleAction* m_viewPreviews;
    KToggleAction* m_viewSegmentLabels;
    KToggleAction* m_enableMIDIrouting;

    KAction *m_playTransport;
    KAction *m_stopTransport;
    KAction *m_rewindTransport;
    KAction *m_ffwdTransport; 
    KAction *m_recordTransport;
    KAction *m_rewindEndTransport;
    KAction *m_ffwdEndTransport;

    KProcess* m_sequencerProcess;
    bool m_sequencerCheckedIn;

#ifdef HAVE_LIBJACK
    KProcess* m_jackProcess;
#endif // HAVE_LIBJACK

    ZoomSlider<double> *m_zoomSlider;
    QLabel             *m_zoomLabel;

    RosegardenProgressBar *m_progressBar;

    // SequenceManager
    //
    Rosegarden::SequenceManager *m_seqManager;

    // Transport dialog pointer
    //
    Rosegarden::RosegardenTransportDialog *m_transport;

    // Dialogs which depend on the document

    // Audio file manager
    //
    Rosegarden::AudioManagerDialog *m_audioManagerDialog;

    bool m_originatingJump;

    // Use these in conjucntion with the loop button to
    // remember where a loop was if we've ever set one.
    Rosegarden::timeT m_storedLoopStart;
    Rosegarden::timeT m_storedLoopEnd;

    bool m_useSequencer;
    bool m_dockVisible;

    Rosegarden::AudioPluginManager *m_pluginManager;

    QTimer* m_autoSaveTimer;

    Rosegarden::Clipboard *m_clipboard;

    SegmentParameterBox		  *m_segmentParameterBox;
    InstrumentParameterBox	  *m_instrumentParameterBox;
    TrackParameterBox             *m_trackParameterBox;

    PlayListDialog        *m_playList;
    DeviceManagerDialog   *m_deviceManager;
    SynthPluginManagerDialog *m_synthManager;
    AudioMixerWindow      *m_audioMixer;
    MidiMixerWindow       *m_midiMixer;
    BankEditorDialog      *m_bankEditor;
    MarkerEditorDialog    *m_markerEditor;
    TempoView             *m_tempoView;
    TriggerSegmentManager *m_triggerSegmentManager;
    std::set<ControlEditorDialog *> m_controlEditors;
    std::map<int, Rosegarden::AudioPluginDialog*> m_pluginDialogs;
#ifdef HAVE_LIBLO
    AudioPluginOSCGUIManager *m_pluginGUIManager;
#endif

    static const char* const MainWindowConfigGroup;

    static RosegardenGUIApp *m_myself;

    // Used to fetch the current sequencer position from the mmapped sequencer information file
    //
    QTimer *m_playTimer;
    QTimer *m_stopTimer;

    StartupTester *m_startupTester;

    RosegardenParameterArea *m_parameterArea;

#ifdef HAVE_LIRC        
    LircClient *m_lircClient;
    LircCommander *m_lircCommander;
#endif     
};

/**
 * Temporarily change the global cursor to waitCursor
 */
class SetWaitCursor
{
public:
    SetWaitCursor();
    ~SetWaitCursor();

protected:
    RosegardenGUIApp* m_guiApp;
    QCursor m_saveCursor;
    QCursor m_saveSegmentCanvasCursor;
};
 
#endif // ROSEGARDENGUI_H
