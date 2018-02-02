//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2002-2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#ifndef __NOTELINE_H__
#define __NOTELINE_H__

#include "textlinebase.h"

namespace Ms {

class Note;

#if 0
//---------------------------------------------------------
//   @@ TextLineSegment
//---------------------------------------------------------

class TextLineSegment : public TextLineBaseSegment {
   protected:

   public:
      TextLineSegment(Score* s) : TextLineBaseSegment(s)  { }
      virtual ElementType type() const override     { return ElementType::TEXTLINE_SEGMENT; }
      virtual TextLineSegment* clone() const override { return new TextLineSegment(*this); }
      TextLine* textLine() const                      { return (TextLine*)spanner(); }
      virtual void layout() override;
      virtual QVariant getProperty(P_ID propertyId) const override;
      virtual bool setProperty(P_ID propertyId, const QVariant&) override;
      virtual QVariant propertyDefault(P_ID) const override;
      virtual PropertyStyle propertyStyle(P_ID) const override;
      virtual void resetProperty(P_ID id) override;
      virtual void styleChanged() override;
      };
#endif

//---------------------------------------------------------
//   @@ NoteLine
//---------------------------------------------------------

class NoteLine final : public TextLineBase {
      Note* _startNote;
      Note* _endNote;

   public:
      NoteLine(Score* s);
      NoteLine(const NoteLine&);
      ~NoteLine() {}

      virtual NoteLine* clone() const         { return new NoteLine(*this); }
      virtual ElementType type() const        { return ElementType::NOTELINE; }

      void setStartNote(Note* n)  { _startNote = n; }
      Note* startNote() const     { return _startNote; }
      void setEndNote(Note* n)    { _endNote = n; }
      Note* endNote() const       { return _endNote; }
      virtual LineSegment* createLineSegment() override;
      };


}     // namespace Ms
#endif
