local peripherals = peripheral.getNames()
print("Connected peripherals:")
for i, name in ipairs(peripherals) do
    print(i .. ": " .. name)
end
