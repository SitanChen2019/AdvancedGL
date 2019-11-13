#include "Constraints.h"
#include "Simulator.h"

namespace PBD{
        void Constraint::solve( Simulator* pSimulator)
        {
            if( mType == ConstraintType::CT_DISTANCE_CONSTRAINT )
            {
                pSimulator->solveConstraint( mDistanceCons );
            }
        }
}