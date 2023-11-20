using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TP4_Simulation
{
	class SimulationBasic
	{
		public long NbMois { get; private set; }
		public long NbLapins { get; private set; }
		public LinkedList<PairLapin> LstLapin { get; private set; }

		public SimulationBasic()
		{
			LstLapin = new LinkedList<PairLapin>();
		}

		public void InitSimulation()
		{
			LstLapin.Clear();
			NbLapins = 0;
			NbMois = 0;

			NouvellePairLapin();
		}

		public void NouvellePairLapin()
		{
			LstLapin.AddFirst(new PairLapin());
			NbLapins++;
		}

		public void PrintLapin()
		{
			Console.WriteLine("Mois n° {0}, {1} paires de lapins.", NbMois, NbLapins);
		}

		public void Simuler(int n)
		{
			for (int i = 0; i < n; i++)
			{
				PrintLapin();
				for (LinkedListNode<PairLapin> l = LstLapin.First; l != null; l = l.Next)
				{
					l.Value.MoisSuivant(this);
				}
				NbMois++;
			}
		}
	}
}
