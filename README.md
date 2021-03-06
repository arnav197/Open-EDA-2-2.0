#This project is the Open EDA code.

#The project is divided into several sub-projects

Bench: Collection of converted netlist versions of ISCAS'85 and ITC'99 benchmarks. 

Update: The ISCAS89 benchmarks are added too along with the perl script used to make them 'pseudo-combonational'.

->Pseudo-Combinational Benchmark: The ITC'99 benchmarks are made 'pseudo-combinational' by replacing each flip-flop with an input (from the flip-flop output) and an output (to the flip-flop input). For more information on the ITC'99 benchmarks, visit https://www.cerc.utexas.edu/itc99-benchmarks/bendoc1.html. For reference regarding ISCAS'85, visit https://davidkebo.com/documents/iscas85.pdf.



PACKAGES: GoogleTest code. Refer to https://github.com/google/googletest for more.

YS.Working.Solution.sln: VS code file combining and binding all 'sub-projects' together.

#Following are the sub-projects involved:

->Parser: Covert a test file into a circuit object.
	
	-Parser: A Parser takes a text string and processes the file it points to. After processing, a circuit object can be returned.

->Structures: Represent the structural relationship of items within a circuit, e.g. lines, nodes, connections, etc.
	
	-Connection: This class models any object which connects to other objects through Connections.
	
	-Level: This class is for structures where the arangement of Connecting objects implies a "level" relative to a given object is created.
	
	-Circuit: A representation of a Circuit.

->Faults: Set faulty value on circuit, create a list of faults of the circuit, set/reset fault list.
	
	-Fault: A modification to a Valued (Faulty) object. A Fault, when activated, will change the Value returned by a Faulty object.
	
	-FaultGenerator: An object capable of populating a list of faults for a Circuit. The base implementation generates stuck-at faults using checkpoint theorem (not applied to XOR gates).
	
	-FaultSimulator: A Simulator capable of performing fault simulation. FaultSimulator builds upon existing event-based Simulators to apply stimulus and measure the respons of circuits, except the same stimulus will be applied multiple times: one for each Fault (plus once for the original circuit).
	
	-FaultStructures: A Line with a potential Fault on it.
	
	-FaultyValue: A FaultyValue extends the functionality of a regular Value by modeling two-frame simulation: the first "frame" represents the effect of a fault in a good (fault-free) circuit whilst the second "frame" represents the state of a circuit in the presence of a fault.

->Simulation: Create simulation structure (simulation line and simulation node), generate a simultor to stimulus circuit and observe outputs.
	
	-EventQueue: Events are the entries which make up an event queue. Each event contains 1) an evented object which needs to be updated and 2) the priority (e.g., the input level in a circuit) of the event relative to other events.
	
	-Function: A representation of a output-generating function. A function takes in a vector of inputs and translates them into an output.
	
	-Net: A Net is a collection of Lines. For most purposes, a Net should be used solely for editing a collection of Lines, i.e., adding or removing Lines in a consistent manner.
	
	-SimulationNet: An extension of Net with explicit simulation functions.
	
	-SimulationStructures: It includes SimulationLine and SimulationNode. SimulationLine is a Line which can be used for basic binary Simulation. SimulationNode is a Node which contains all parts needed for event-based simulation.
	
	-Simulator: A simulator which applies stimulus to a circuit and observes outputs. A Simulator is any object which has the ability to apply stimulus to an object.
	
	-Value: A simultable value. A Value holds a value created during simulation. As opposed to raw data types (bool, char, float, etc.), a Value holds extra informationa and has support functions.
	
	-ValueVectorFunctions: A static container for Value-containing vector manipulation.
	
->COP: Create COP structure (COP line and COP node); calcultate Controllability and observability information.
	
	-COPSturctures: It includes COP, COPLine and COPNode. COP is a pure virtual class. Inheriting objects must designate how controllability and obserability are calculated. COPLine is a Line which holds information necessary to perform COP. COPNode is a Node which holds information necessary to perform COP.

->TPI: Generate a list of testpoints for the circuit; active/deactive a TP(control tp, observe tp, invert tp); perform TPI algorithm for selecting TPs.
	
	-COP_fault_calculator: Calculates the fault coverage of a circuit using COP values.
	
	-Moghaddam_Structures: This object has the ability to "propogate" faults by way of the Moghaddam algorithm. For the specifics of the algorithm, see the article: https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=835591
	
	-Testpoint:  An object capable of modifying a circuit when activated. This class is pure virtual. Other methods must define the actions corresponding to activation. A testpoint is "valued" in the sense that it will return value of its location (presuming it is active).
	
	-Testpoint_control: A control testpoint (both control-0 and control-1).
	
	-Testpoint_invert: An invert testpoint.
	
	-Testpoint_observe: An observe testpoint.
	
	-TPGenerator: Create all TPs for a circuit.
	
	-TPI: An algroithm which implements iterative test point insertion (TPI). This is a pure-virtual class describing the features of any TPI algorithm.
	
	-TPI_COP: COP-based TPI. Method is similar as H.-C. Tsai, K.-T. Cheng, C.-J. Lin and S. Bhawmik. A hybrid algorithm for test point selection for scan-based BIST. Proc. of 34th Design Automation Conference, pages 478-483,1997.

->PRPG: A peudo-random pattern generator (PRPG). 
	
	-PRPG: The LFSR implemented here is a 31-bit external LFSR. This implies sequential vectors generated by the PRPG are highly correlated to eachother, as they will be the shifted result of the previous vector with a single bit difference.

->Trace: Trace forward and backwards in a circuit and return the requested elements.
	
	-Tracable: This object is capible of "tracing" forward and backwards and returning related objects.
	
	-Tracer:  A tracer is able to trace forward and backwards in a circuit and return the requested elements.
	
->SAT: Perform SAT
	
	-Backtracer: A backtracer takes one (or more) lines and values in a circuit and will return 1) a PI assignment which attempts to make the values occur and 2) a list of implied assignments made inside the circuit.
	
	-Combination: A Combination is a special type of 'Node' which represents a combination of particular objects in a circuit at particular Values.
	
	-Goal: When performing any satisfiability(-like) problem, a Goal is a "final" Objective to satisfy. Once a Goal is met, it will tell the SAT algorithm to stop. Alternatively, a Goal can tell when failure is inevitable, and will tell the SAT algorithm to stop.
	
	-ImplicationStack:  Alternative Events are extensions to EventQueue events (no functions are overwritten). They allow possible "backtrace" (i.e., undo) values to be tracked and they allow alternative values to be tracked
	
	-SAT: Performs satisfiability (SAT) on Circuit Line-Value combinations.
	
	-SATStructures: It includes SATNode and SATLine. SatNode is the combination of Node elements required to perform SAT. SATLine is a Line used in conjunction with SATNode.

->ATPG: Perform ATPG, it include ATPGStrucutres and DFronier.
	
	-ATPGStrucutres: It includes ATPGNode and ATPGLine. ATPGNode is the combination of Nodes required to perfrom ATPG. ATPGLine is a Line used in conjunction with ATPGNode.
	
	-DFronier: A D-Frontier is a type of "Goal" used during SAT. In effect, this turns SAT into an ATPG algorithm, with the specific ATPG being dictated by the type of Backtracer algorithm inside SAT.

->Expand: Convert a circuit into a certain format, but not change the function. 
	
	-Expand: An object to convert circuit gate inputs and fanouts, which makes gate input number no more than two and gate fan-out number no more than two.
	
->Window: Generate a sub-circuit and get input features for Artificial Neural Networks.
	
	-Window: Extract a sub-circuit from a benchmark circuit; generate Artificial Neural Network input features in vectorized format.
