using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TP4_Simulation
{
	class Program
	{
		static void Main(string[] args)
		{
			SimulationBasic s = new SimulationBasic();
			s.InitSimulation();
			s.Simuler(35);

			Console.ReadLine();
		}
	}
}
