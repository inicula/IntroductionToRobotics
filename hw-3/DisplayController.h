#pragma once
#include "JoystickController.h"

class DisplayController {
public:
    enum class State : u8 {
        Disengaged = 0,
        Engaged,
    };
    enum Segment : u8 {
        A = 0,
        B,
        C,
        D,
        E,
        F,
        G,
        DP,
        NumSegments,
    };

    using SegNeighbours = Segment[JoystickController::NUM_DIRECTIONS];
    using Bitfield16 = uint16_t;

    DisplayController() = default;
    void update(u32, JoystickController&);
    void init() const;

    static constexpr SegNeighbours SEGMENTS_NEIGHBOURS[NumSegments] = {
     /* Source segment                       Neighbour through move type                     */
     /*                    None          Up            Down          Left        Right       */
        [Segment::A]   = { Segment::A,   Segment::A,   Segment::G,   Segment::F, Segment::B  },
        [Segment::B]   = { Segment::B,   Segment::A,   Segment::G,   Segment::F, Segment::B  },
        [Segment::C]   = { Segment::C,   Segment::G,   Segment::D,   Segment::E, Segment::DP },
        [Segment::D]   = { Segment::D,   Segment::G,   Segment::D,   Segment::E, Segment::C  },
        [Segment::E]   = { Segment::E,   Segment::G,   Segment::D,   Segment::E, Segment::C  },
        [Segment::F]   = { Segment::F,   Segment::A,   Segment::G,   Segment::F, Segment::B  },
        [Segment::G]   = { Segment::G,   Segment::A,   Segment::D,   Segment::G, Segment::G  },
        [Segment::DP]  = { Segment::DP,  Segment::DP,  Segment::DP,  Segment::C, Segment::DP },
    };
    static constexpr u8 SEGMENT_PINS[NumSegments] = {
        [Segment::A] = 4,
        [Segment::B] = 5,
        [Segment::C] = 6,
        [Segment::D] = 7,
        [Segment::E] = 8,
        [Segment::F] = 9,
        [Segment::G] = 10,
        [Segment::DP] = 11,
    };

private:
    static void drawSegments(Bitfield16);

private:
    Segment currentSegment;
    State currentState;
    Bitfield16 segmentStates;
};
