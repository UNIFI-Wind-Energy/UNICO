# UNIversity of Florence Research COntroller (UNICO)

Research controller for stall-regulated wind turbines developed by the University of Florence (Italy). 

Key features: 

- Choice of TSR-tracking (P-I) or Kω2 law for torque control in Region 2
- Robust switching between Region 1 and 2 and possibility to impose minimum operating rpm
- P-I torque control in Region 3. Possibility to impose a wind speed dependent rotor speed set-point for fine-tuning of the generator torque curve
- Fully compatible with aero-elastic tools that support Bladed-style control interface such as OpenFAST, QBlade, Bladed



Contents of this repository: 

- Src: controller in binary (.dll) GH-bladed style format (Compatible and tested with OpenFAST) and in Simulink format. Simulink format controller can be used with OpenFAST through S-Function. More information can be found in the "Src" directory. 
- Test_cases: Set of ready-to-use test cases. All test cases based on the UNIFI 50kW SRWT design (https://github.com/REASE-UNIFI/UNIFI-50kW-RWT - CCBY4.0-SA)
- Matlab_toolbox: I/O utilities. Reccommended to work with Simulink variant of the controller. Modified from ROSCO Controller toolbox (https://github.com/NREL/ROSCO - Apache 2.0 License)


<p xmlns:cc="http://creativecommons.org/ns#" >This work is licensed under <a href="http://creativecommons.org/licenses/by-nc-sa/4.0/?ref=chooser-v1" target="_blank" rel="license noopener noreferrer" style="display:inline-block;">CC BY-NC-SA 4.0<img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/cc.svg?ref=chooser-v1"><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/by.svg?ref=chooser-v1"><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/nc.svg?ref=chooser-v1"><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/sa.svg?ref=chooser-v1"></a></p>
