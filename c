-- Autopilot for airships or other hovering craft for newer versions vs:cc
-- Tested in 1.20.1 fabric
-- Some figures can be changed in the script below (see comment after the -- marks)
-- Keep in mind that the position of your ship can be different from your position.
-- By Arjen de Vries

term.clear()
term.setCursorPos(1,1)

print("Autopilot by Arjen de Vries")
print("")

-- Find the ship peripheral
local ship = peripheral.find("vs_ship") or peripheral.find("ship") or peripheral.find("vs") -- try common names

if not ship then
    print("ERROR: Ship peripheral not found! Please connect the ship controller peripheral.")
    return -- exit script if no ship found
end

print("Ship peripheral found.")

-- Optional: list ship methods for debugging
print("Available ship methods:")
for k,v in pairs(ship) do
    print(" - " .. k)
end

print("Enter X coordinate: ")
local destX = tonumber(read())

print("Enter Z coordinate: ")
local destZ = tonumber(read())

print("Enter cruise altitude: ")
local cruiseAlt = tonumber(read())

print("Destination altitude: ")
local destAlt = tonumber(read())

term.clear()
term.setCursorPos(1,1)

print("Autopilot engaged")

function lowerShip()
    local monitor = peripheral.find("monitor")
    if monitor then
        monitor.setTextScale(0.5)
        monitor.clear()
        monitor.setCursorPos(1,1)
        monitor.write("Descending to: ")
        monitor.setCursorPos(1,2)
        monitor.write(destAlt)
    end

    local pos = ship.getWorldspacePosition()
    repeat
        local mass = ship.getMass()
        -- -12 in line below can be changed to descend faster/slower
        ship.applyRotDependentForce(0, -12 * mass, 0)
        sleep(0)
        pos = ship.getWorldspacePosition()
    until(pos.y < tonumber(destAlt))
    return true
end

function raiseShip()
    local monitor = peripheral.find("monitor")
    if monitor then
        monitor.setTextScale(0.5)
        monitor.setCursorPos(1,2)
        monitor.write("Climbing to: ")
        monitor.setCursorPos(1,3)
        monitor.write(cruiseAlt)
    end

    local pos = ship.getWorldspacePosition()
    repeat
        local mass = ship.getMass()
        -- 12 in line below can be changed to ascend faster/slower
        ship.applyRotDependentForce(0, 12 * mass, 0)
        sleep(0)
        pos = ship.getWorldspacePosition()
    until(pos.y > tonumber(cruiseAlt))
    return true
end

function getYaw()
    local rot = ship.getQuaternion()
    local shipYaw = math.atan2(2*rot.y*rot.w - 2*rot.x*rot.z, 1 - 2*rot.y*rot.y - 2*rot.z*rot.z)
    return shipYaw
end

function getIdealYaw()
    local pos = ship.getWorldspacePosition()
    local idealYaw = math.atan2(pos.x - destX, pos.z - destZ)
    return idealYaw
end

function getDistanceFromDestination()
    local pos = ship.getWorldspacePosition()
    local distanceX = destX - pos.x
    local distanceZ = destZ - pos.z
    local distance = math.sqrt(distanceX*distanceX + distanceZ*distanceZ)
    return distance
end

function cw()
    local mass = ship.getMass()
    -- -1000 in line below can be changed to rotate right faster/slower
    ship.applyRotDependentTorque(0, -1000 * mass, 0)
    sleep(0)
end

function ccw()
    local mass = ship.getMass()
    -- 1000 in line below can be changed to rotate left faster/slower
    ship.applyRotDependentTorque(0, 1000 * mass, 0)
    sleep(0)
end

function rotateShip()
    local facing = false
    while not facing do
        local idealYaw = getIdealYaw()
        local shipYaw = getYaw()

        if shipYaw > idealYaw - 0.2 and shipYaw < idealYaw + 0.2 then
            facing = true
        else
            local sCalc = shipYaw + math.pi
            local iCalc = idealYaw + math.pi
            local dCalc = math.abs(sCalc - iCalc)

            if dCalc > math.pi then
                if sCalc < iCalc then
                    cw()
                elseif sCalc > iCalc then
                    ccw()
                end
            else
                if sCalc > iCalc then
                    cw()
                elseif sCalc < iCalc then
                    ccw()
                end
            end
        end
    end
end

function moveShip()
    local distance = getDistanceFromDestination()
    -- max forward speed can be changed below
    local spd = 7
    repeat
        rotateShip()

        if distance < 150 then
            -- Reduced approach speed can be set below
            spd = 3
        end

        local mass = ship.getMass()
        local vel = ship.getVelocity()
        local speed = math.sqrt(vel.x^2 + vel.z^2)

        if speed < spd then
            -- -70 in line below can be changed to accelerate faster/slower
            ship.applyRotDependentForce(0, 0, -70 * mass)
            sleep(0)
        end
        sleep(0)
        distance = getDistanceFromDestination()

        -- MONITOR BLOCK

        -- calculate remaining travel time (RTT)
        local time = distance / (30 * speed)
        local days = math.floor(time / 24)
        local remaining = time / 24 - days
        local hours = math.floor(remaining * 24)
        remaining = remaining * 24 - hours
        local minutes = math.floor(remaining * 60)
        local monitor = peripheral.find("monitor")

        -- write info to monitor
        if monitor then
            monitor.setTextScale(0.5)
            monitor.clear()

            monitor.setCursorPos(1, 1)
            monitor.write("Cruising")
            monitor.setCursorPos(1, 3)
            monitor.write("Speed   : " .. math.floor(speed + 0.5))
            monitor.setCursorPos(1, 4)
            monitor.write("Distance: " .. math.floor(distance + 0.5))
            monitor.setCursorPos(1, 6)
            monitor.write("RTT: " .. days .. "d " .. hours .. "h " .. minutes .. "m")
        end
    until(distance < 15)
end

local monitor = peripheral.find("monitor")
if monitor then
    monitor.setTextScale(0.5)
    monitor.clear()
    monitor.setCursorPos(1, 1)
    monitor.write("Stand by...")
end

sleep(3)
raiseShip()
sleep(2)

if monitor then
    monitor.clear()
    monitor.setCursorPos(1, 1)
    monitor.write("Stand by...")
    monitor.setTextScale(0.5)
    monitor.setCursorPos(1, 2)
    monitor.write("Set heading")
end

rotateShip()
sleep(2)
moveShip()
sleep(2)
lowerShip()

term.clear()
term.setCursorPos(1,1)

print("Destination reached!")
print("Autopilot disengaged.")

if monitor then
    monitor.setTextScale(0.5)
    monitor.clear()
    monitor.setCursorPos(1,1)
    monitor.write("Destination")
    monitor.setCursorPos(1,2)
    monitor.write("reached!")
    monitor.setCursorPos(1,3)
    monitor.write("Autopilot")
    monitor.setCursorPos(1,4)
    monitor.write("disengaged.")
end
