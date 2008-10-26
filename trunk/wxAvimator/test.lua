-- this is a testing script

app = ScriptEngine.getMainApplication()
app:Show(true)
avw = app.animVwr
--avw = ScriptEngine.getAnimationView()
ani = avw:getAnimation()

for i=1,1000 do
app:Show(true)
 avw:render()
 ani:stepForward()
end

an_new = ScriptEngine.Animation("./animations/avatar_dance3.bvh")
avw:setAnimation(an_new)

ani = avw:getAnimation()
for i=1,1000 do
app:Show(true)
 avw:render()
 ani:stepForward()
end

print(app)
print(avw)
print(ani)

--[[while true do
end]]

