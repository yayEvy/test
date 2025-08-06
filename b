-- Main program loop
local function main()
    while true do
        displayMenu()
        write("Select an option (1-5): ")
        local choice = tonumber(read())

        if choice >= 1 and choice <= 4 then
            -- Use saved coordinates
            local selectedCoord = savedCoords[choice]
            print(string.format("Selected: x=%d, y=%d, z=%d", selectedCoord.x, selectedCoord.y, selectedCoord.z))
        elseif choice == 5 then
            -- Input custom coordinates
            print("Input custom coordinates:")
            local customCoord = getCustomCoords()
            print(string.format("Custom Coordinates: x=%d, y=%d, z=%d", customCoord.x, customCoord.y, customCoord.z))
        else
            print("Invalid option. Try again.")
        end

        -- Autopilot stage
        write("Do you want to autopilot to these coordinates? (yes/no): ")
        local confirm = read()
        if confirm:lower() == "yes" then
            print("Starting autopilot...")
            break  -- Exit the program (or handle autopilot functionality here)
        else
            print("Returning to menu...")
        end
    end
end

-- Run the program
main()
 
