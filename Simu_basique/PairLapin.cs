using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TP4_Simulation
{
	class PairLapin
	{
		public bool Adult { get; private set; }

		public PairLapin()
		{
			Adult = false;
		}

		public void MoisSuivant(SimulationBasic s)
		{
			if (Adult)
			{
				s.NouvellePairLapin();
			}
			else
			{
				Adult = true;
			}
		}

	}
}
