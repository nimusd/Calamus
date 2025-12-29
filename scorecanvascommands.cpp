#include "scorecanvascommands.h"
#include "scorecanvas.h"
#include <QDebug>

// ============================================================================
// Add Note Command
// ============================================================================

AddNoteCommand::AddNoteCommand(Phrase *phrase, const Note &note, ScoreCanvas *canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_phrase(phrase)
    , m_note(note)
    , m_canvas(canvas)
    , m_firstTime(true)
{
    setText("Add Note");
}

void AddNoteCommand::undo()
{
    // Remove the last note (the one we added)
    QVector<Note> &notes = m_phrase->getNotes();
    if (!notes.isEmpty()) {
        notes.removeLast();
        m_canvas->update();
        qDebug() << "Undo: Note removed";
    }
}

void AddNoteCommand::redo()
{
    // Add the note
    m_phrase->addNote(m_note);
    m_canvas->update();
    qDebug() << "Redo: Note added";
}

// ============================================================================
// Delete Note Command
// ============================================================================

DeleteNoteCommand::DeleteNoteCommand(Phrase *phrase, int noteIndex, ScoreCanvas *canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_phrase(phrase)
    , m_noteIndex(noteIndex)
    , m_canvas(canvas)
{
    // Store the note before it's deleted
    const QVector<Note> &notes = m_phrase->getNotes();
    if (noteIndex >= 0 && noteIndex < notes.size()) {
        m_note = notes[noteIndex];
    }
    setText("Delete Note");
}

void DeleteNoteCommand::undo()
{
    // Re-insert the note at its original position
    QVector<Note> &notes = m_phrase->getNotes();
    notes.insert(m_noteIndex, m_note);
    m_canvas->update();
    qDebug() << "Undo: Note restored at index" << m_noteIndex;
}

void DeleteNoteCommand::redo()
{
    // Remove the note at the stored index
    m_phrase->removeNoteByIndex(m_noteIndex);
    m_canvas->update();
    qDebug() << "Redo: Note deleted at index" << m_noteIndex;
}

// ============================================================================
// Move Note Command
// ============================================================================

MoveNoteCommand::MoveNoteCommand(Phrase *phrase, int noteIndex,
                                 double oldStartTime, double oldPitch,
                                 double newStartTime, double newPitch,
                                 const Curve &oldPitchCurve, const Curve &newPitchCurve,
                                 bool hasPitchCurve, ScoreCanvas *canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_phrase(phrase)
    , m_noteIndex(noteIndex)
    , m_oldStartTime(oldStartTime)
    , m_oldPitch(oldPitch)
    , m_newStartTime(newStartTime)
    , m_newPitch(newPitch)
    , m_oldPitchCurve(oldPitchCurve)
    , m_newPitchCurve(newPitchCurve)
    , m_hasPitchCurve(hasPitchCurve)
    , m_canvas(canvas)
{
    setText("Move Note");
}

void MoveNoteCommand::undo()
{
    QVector<Note> &notes = m_phrase->getNotes();
    if (m_noteIndex >= 0 && m_noteIndex < notes.size()) {
        Note &note = notes[m_noteIndex];
        note.setStartTime(m_oldStartTime);

        if (m_hasPitchCurve) {
            note.setPitchCurve(m_oldPitchCurve);
        } else {
            note.setPitchHz(m_oldPitch);
        }

        m_canvas->update();
        qDebug() << "Undo: Note moved to" << m_oldStartTime << "ms," << m_oldPitch << "Hz";
    }
}

void MoveNoteCommand::redo()
{
    QVector<Note> &notes = m_phrase->getNotes();
    if (m_noteIndex >= 0 && m_noteIndex < notes.size()) {
        Note &note = notes[m_noteIndex];
        note.setStartTime(m_newStartTime);

        if (m_hasPitchCurve) {
            note.setPitchCurve(m_newPitchCurve);
        } else {
            note.setPitchHz(m_newPitch);
        }

        m_canvas->update();
        qDebug() << "Redo: Note moved to" << m_newStartTime << "ms," << m_newPitch << "Hz";
    }
}

bool MoveNoteCommand::mergeWith(const QUndoCommand *other)
{
    // Merge consecutive move commands for the same note
    if (other->id() != id()) {
        return false;
    }

    const MoveNoteCommand *moveCommand = static_cast<const MoveNoteCommand*>(other);
    if (moveCommand->m_noteIndex != m_noteIndex) {
        return false;
    }

    // Update the new position to the latest
    m_newStartTime = moveCommand->m_newStartTime;
    m_newPitch = moveCommand->m_newPitch;
    m_newPitchCurve = moveCommand->m_newPitchCurve;

    return true;
}

// ============================================================================
// Resize Note Command
// ============================================================================

ResizeNoteCommand::ResizeNoteCommand(Phrase *phrase, int noteIndex,
                                     double oldStartTime, double oldDuration,
                                     double newStartTime, double newDuration,
                                     ScoreCanvas *canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_phrase(phrase)
    , m_noteIndex(noteIndex)
    , m_oldStartTime(oldStartTime)
    , m_oldDuration(oldDuration)
    , m_newStartTime(newStartTime)
    , m_newDuration(newDuration)
    , m_canvas(canvas)
{
    setText("Resize Note");
}

void ResizeNoteCommand::undo()
{
    QVector<Note> &notes = m_phrase->getNotes();
    if (m_noteIndex >= 0 && m_noteIndex < notes.size()) {
        Note &note = notes[m_noteIndex];
        note.setStartTime(m_oldStartTime);
        note.setDuration(m_oldDuration);
        m_canvas->update();
        qDebug() << "Undo: Note resized to" << m_oldStartTime << "ms," << m_oldDuration << "ms duration";
    }
}

void ResizeNoteCommand::redo()
{
    QVector<Note> &notes = m_phrase->getNotes();
    if (m_noteIndex >= 0 && m_noteIndex < notes.size()) {
        Note &note = notes[m_noteIndex];
        note.setStartTime(m_newStartTime);
        note.setDuration(m_newDuration);
        m_canvas->update();
        qDebug() << "Redo: Note resized to" << m_newStartTime << "ms," << m_newDuration << "ms duration";
    }
}

bool ResizeNoteCommand::mergeWith(const QUndoCommand *other)
{
    // Merge consecutive resize commands for the same note
    if (other->id() != id()) {
        return false;
    }

    const ResizeNoteCommand *resizeCommand = static_cast<const ResizeNoteCommand*>(other);
    if (resizeCommand->m_noteIndex != m_noteIndex) {
        return false;
    }

    // Update the new size to the latest
    m_newStartTime = resizeCommand->m_newStartTime;
    m_newDuration = resizeCommand->m_newDuration;

    return true;
}

// ============================================================================
// Edit Curve Command
// ============================================================================

EditCurveCommand::EditCurveCommand(Phrase *phrase, int noteIndex, CurveType curveType,
                                   const Curve &oldCurve, const Curve &newCurve,
                                   ScoreCanvas *canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_phrase(phrase)
    , m_noteIndex(noteIndex)
    , m_curveType(curveType)
    , m_oldCurve(oldCurve)
    , m_newCurve(newCurve)
    , m_canvas(canvas)
{
    if (curveType == DynamicsCurve) {
        setText("Edit Dynamics Curve");
    } else {
        setText("Edit Bottom Curve");
    }
}

void EditCurveCommand::undo()
{
    QVector<Note> &notes = m_phrase->getNotes();
    if (m_noteIndex >= 0 && m_noteIndex < notes.size()) {
        Note &note = notes[m_noteIndex];

        if (m_curveType == DynamicsCurve) {
            note.setDynamicsCurve(m_oldCurve);
        } else {
            note.setBottomCurve(m_oldCurve);
        }

        m_canvas->update();
        qDebug() << "Undo: Curve edited";
    }
}

void EditCurveCommand::redo()
{
    QVector<Note> &notes = m_phrase->getNotes();
    if (m_noteIndex >= 0 && m_noteIndex < notes.size()) {
        Note &note = notes[m_noteIndex];

        if (m_curveType == DynamicsCurve) {
            note.setDynamicsCurve(m_newCurve);
        } else {
            note.setBottomCurve(m_newCurve);
        }

        m_canvas->update();
        qDebug() << "Redo: Curve edited";
    }
}

bool EditCurveCommand::mergeWith(const QUndoCommand *other)
{
    // Merge consecutive curve edits for the same note and curve type
    if (other->id() != id()) {
        return false;
    }

    const EditCurveCommand *editCommand = static_cast<const EditCurveCommand*>(other);
    if (editCommand->m_noteIndex != m_noteIndex || editCommand->m_curveType != m_curveType) {
        return false;
    }

    // Update the new curve to the latest
    m_newCurve = editCommand->m_newCurve;

    return true;
}

// ============================================================================
// Delete Multiple Notes Command
// ============================================================================

DeleteMultipleNotesCommand::DeleteMultipleNotesCommand(Phrase *phrase, const QVector<int> &noteIndices, ScoreCanvas *canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_phrase(phrase)
    , m_canvas(canvas)
{
    // Store notes with their indices before deletion (in descending order for proper restoration)
    QVector<int> sortedIndices = noteIndices;
    std::sort(sortedIndices.begin(), sortedIndices.end(), std::greater<int>());

    const QVector<Note> &notes = m_phrase->getNotes();
    for (int index : sortedIndices) {
        if (index >= 0 && index < notes.size()) {
            m_notesWithIndices.append(qMakePair(index, notes[index]));
        }
    }

    if (noteIndices.size() == 1) {
        setText("Delete Note");
    } else {
        setText(QString("Delete %1 Notes").arg(noteIndices.size()));
    }
}

void DeleteMultipleNotesCommand::undo()
{
    // Re-insert notes at their original positions (in reverse order since we stored descending)
    for (int i = m_notesWithIndices.size() - 1; i >= 0; --i) {
        int index = m_notesWithIndices[i].first;
        const Note &note = m_notesWithIndices[i].second;

        QVector<Note> &notes = m_phrase->getNotes();
        notes.insert(index, note);
    }

    m_canvas->update();
    qDebug() << "Undo: Restored" << m_notesWithIndices.size() << "notes";
}

void DeleteMultipleNotesCommand::redo()
{
    // Delete notes in descending order to avoid index shifting
    for (const auto &pair : m_notesWithIndices) {
        int index = pair.first;
        m_phrase->removeNoteByIndex(index);
    }

    m_canvas->update();
    qDebug() << "Redo: Deleted" << m_notesWithIndices.size() << "notes";
}

// ============================================================================
// Move Multiple Notes Command
// ============================================================================

MoveMultipleNotesCommand::MoveMultipleNotesCommand(Phrase *phrase, const QVector<int> &noteIndices,
                                                   const QVector<NoteState> &oldStates,
                                                   const QVector<NoteState> &newStates,
                                                   ScoreCanvas *canvas, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_phrase(phrase)
    , m_oldStates(oldStates)
    , m_newStates(newStates)
    , m_canvas(canvas)
{
    if (noteIndices.size() == 1) {
        setText("Move Note");
    } else {
        setText(QString("Move %1 Notes").arg(noteIndices.size()));
    }
}

void MoveMultipleNotesCommand::undo()
{
    QVector<Note> &notes = m_phrase->getNotes();

    // Restore old states
    for (const NoteState &state : m_oldStates) {
        if (state.index >= 0 && state.index < notes.size()) {
            Note &note = notes[state.index];
            note.setStartTime(state.startTime);

            if (state.hasPitchCurve) {
                note.setPitchCurve(state.pitchCurve);
            } else {
                note.setPitchHz(state.pitch);
            }
        }
    }

    m_canvas->update();
    qDebug() << "Undo: Restored" << m_oldStates.size() << "notes to original positions";
}

void MoveMultipleNotesCommand::redo()
{
    QVector<Note> &notes = m_phrase->getNotes();

    // Apply new states
    for (const NoteState &state : m_newStates) {
        if (state.index >= 0 && state.index < notes.size()) {
            Note &note = notes[state.index];
            note.setStartTime(state.startTime);

            if (state.hasPitchCurve) {
                note.setPitchCurve(state.pitchCurve);
            } else {
                note.setPitchHz(state.pitch);
            }
        }
    }

    m_canvas->update();
    qDebug() << "Redo: Moved" << m_newStates.size() << "notes";
}
