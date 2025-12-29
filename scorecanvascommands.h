#ifndef SCORECANVASCOMMANDS_H
#define SCORECANVASCOMMANDS_H

#include <QUndoCommand>
#include "note.h"
#include "curve.h"
#include "phrase.h"

// Forward declaration
class ScoreCanvas;

// ============================================================================
// Add Note Command
// ============================================================================
class AddNoteCommand : public QUndoCommand
{
public:
    AddNoteCommand(Phrase *phrase, const Note &note, ScoreCanvas *canvas, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Phrase *m_phrase;
    Note m_note;
    ScoreCanvas *m_canvas;
    bool m_firstTime;
};

// ============================================================================
// Delete Note Command
// ============================================================================
class DeleteNoteCommand : public QUndoCommand
{
public:
    DeleteNoteCommand(Phrase *phrase, int noteIndex, ScoreCanvas *canvas, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Phrase *m_phrase;
    Note m_note;
    int m_noteIndex;
    ScoreCanvas *m_canvas;
};

// ============================================================================
// Move Note Command
// ============================================================================
class MoveNoteCommand : public QUndoCommand
{
public:
    MoveNoteCommand(Phrase *phrase, int noteIndex, double oldStartTime, double oldPitch,
                    double newStartTime, double newPitch,
                    const Curve &oldPitchCurve, const Curve &newPitchCurve,
                    bool hasPitchCurve, ScoreCanvas *canvas, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    int id() const override { return 1; }  // For command merging
    bool mergeWith(const QUndoCommand *other) override;

private:
    Phrase *m_phrase;
    int m_noteIndex;
    double m_oldStartTime;
    double m_oldPitch;
    double m_newStartTime;
    double m_newPitch;
    Curve m_oldPitchCurve;
    Curve m_newPitchCurve;
    bool m_hasPitchCurve;
    ScoreCanvas *m_canvas;
};

// ============================================================================
// Resize Note Command
// ============================================================================
class ResizeNoteCommand : public QUndoCommand
{
public:
    ResizeNoteCommand(Phrase *phrase, int noteIndex,
                     double oldStartTime, double oldDuration,
                     double newStartTime, double newDuration,
                     ScoreCanvas *canvas, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    int id() const override { return 2; }  // For command merging
    bool mergeWith(const QUndoCommand *other) override;

private:
    Phrase *m_phrase;
    int m_noteIndex;
    double m_oldStartTime;
    double m_oldDuration;
    double m_newStartTime;
    double m_newDuration;
    ScoreCanvas *m_canvas;
};

// ============================================================================
// Edit Curve Command
// ============================================================================
class EditCurveCommand : public QUndoCommand
{
public:
    enum CurveType {
        DynamicsCurve,
        BottomCurve
    };

    EditCurveCommand(Phrase *phrase, int noteIndex, CurveType curveType,
                    const Curve &oldCurve, const Curve &newCurve,
                    ScoreCanvas *canvas, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    int id() const override { return 3; }  // For command merging
    bool mergeWith(const QUndoCommand *other) override;

private:
    Phrase *m_phrase;
    int m_noteIndex;
    CurveType m_curveType;
    Curve m_oldCurve;
    Curve m_newCurve;
    ScoreCanvas *m_canvas;
};

// ============================================================================
// Delete Multiple Notes Command (for multi-selection delete)
// ============================================================================
class DeleteMultipleNotesCommand : public QUndoCommand
{
public:
    DeleteMultipleNotesCommand(Phrase *phrase, const QVector<int> &noteIndices, ScoreCanvas *canvas, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Phrase *m_phrase;
    QVector<QPair<int, Note>> m_notesWithIndices;  // Store notes with their original indices
    ScoreCanvas *m_canvas;
};

// ============================================================================
// Move Multiple Notes Command (for multi-selection drag)
// ============================================================================
class MoveMultipleNotesCommand : public QUndoCommand
{
public:
    struct NoteState {
        int index;
        double startTime;
        double pitch;
        Curve pitchCurve;
        bool hasPitchCurve;
    };

    MoveMultipleNotesCommand(Phrase *phrase, const QVector<int> &noteIndices,
                            const QVector<NoteState> &oldStates,
                            const QVector<NoteState> &newStates,
                            ScoreCanvas *canvas, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Phrase *m_phrase;
    QVector<NoteState> m_oldStates;
    QVector<NoteState> m_newStates;
    ScoreCanvas *m_canvas;
};

#endif // SCORECANVASCOMMANDS_H
