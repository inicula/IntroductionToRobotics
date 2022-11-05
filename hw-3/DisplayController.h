#pragma once
#include "JoystickController.h"

class DisplayController {
public:
    enum class State : u8 {
        Disengaged = 0,
        Engaged,
    };
    enum Node : u8 {
        A = 0,
        B,
        C,
        D,
        E,
        F,
        G,
        DP,
        NumNodes,
    };

    using NodeNeighbours = Node[JoystickController::NUM_DIRECTIONS];
    using Bitset8 = u8;

    DisplayController() = default;
    void update(u32, JoystickController&);
    void init() const;

    static constexpr NodeNeighbours NODE_NEIGHBOURS[NumNodes] = {
   /*   Source node             Neighbour through move type            */
   /*                  None      Up        Down      Left     Right    */
        [Node::A]  = { Node::A,  Node::A,  Node::G,  Node::F, Node::B  },
        [Node::B]  = { Node::B,  Node::A,  Node::G,  Node::F, Node::B  },
        [Node::C]  = { Node::C,  Node::G,  Node::D,  Node::E, Node::DP },
        [Node::D]  = { Node::D,  Node::G,  Node::D,  Node::E, Node::C  },
        [Node::E]  = { Node::E,  Node::G,  Node::D,  Node::E, Node::C  },
        [Node::F]  = { Node::F,  Node::A,  Node::G,  Node::F, Node::B  },
        [Node::G]  = { Node::G,  Node::A,  Node::D,  Node::G, Node::G  },
        [Node::DP] = { Node::DP, Node::DP, Node::DP, Node::C, Node::DP },
    };
    static constexpr u8 NODE_PINS[NumNodes] = {
        [Node::A] = 4,
        [Node::B] = 5,
        [Node::C] = 6,
        [Node::D] = 7,
        [Node::E] = 8,
        [Node::F] = 9,
        [Node::G] = 10,
        [Node::DP] = 11,
    };

private:
    static void drawNodes(Bitset8);

private:
    Node currentNode;
    State currentState;
    Bitset8 nodeStates;
};
