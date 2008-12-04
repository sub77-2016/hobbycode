sim=al.SimulationEngine()

sim:init()

sim:setUpdateMode(al.SimulationEngine_SIMULATE_REAL_TIME_MULTIPLE, 1)
--sim:setUpdateMode(al.SimulationEngine_SIMULATE_CONSTANT_CHUNK, 0.0333667)

simtr = sim:getSimulator()
cam = sim:getCamera()

simtr:setGravity(al.Vec3r(0, -30, 0))
simtr:setStepSize(0.01)

cam:setPosition(al.Point3r(0, 20, 30))
cam:lookAt(al.Point3r(0, 0, 0))

sim:go()

--[[
while true do
	sim:update(0,0)
end
--]]