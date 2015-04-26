function pass_cards()
    print("I have " .. tostring(hearts.hand_size()) .. " cards!");
    return nil, 1, true, 1;
end

print(hearts.broken());
hearts.blargh(1);
--hearts.log("Testing", tostring(true), tostring(2), tostring(3));
hearts.log(1, 2, "3");
hearts.log("AGAIN");
hearts.pass(pass_cards);
