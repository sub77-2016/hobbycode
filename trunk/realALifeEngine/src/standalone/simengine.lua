sim=al.SimulationEngine()

print(sim.SIMULATE_CONSTANT_CHUNK)

sim:init()

sim:setUpdateMode(al.SimulationEngine_SIMULATE_REAL_TIME_MULTIPLE, 1)
--sim:setUpdateMode(al.SimulationEngine_SIMULATE_CONSTANT_CHUNK, 0.0333667)

sim:getSimulator():setGravity(al.Vec3r(0, -30, 0))
sim:getSimulator():setStepSize(0.01)

sim:getCamera():setPosition(al.Point3r(0, 20, 30))
sim:getCamera():lookAt(al.Point3r(0, 0, 0))

sim:go()

while true do
	sim:update(0,0)
end
