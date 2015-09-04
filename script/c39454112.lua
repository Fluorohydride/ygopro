--出たら目
function c39454112.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--dice
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_TOSS_DICE_NEGATE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c39454112.diceop)
	c:RegisterEffect(e2)
end
function c39454112.diceop(e,tp,eg,ep,ev,re,r,rp)
	local cc=Duel.GetCurrentChain()
	local cid=Duel.GetChainInfo(cc,CHAININFO_CHAIN_ID)
	if c39454112[0]~=cid and Duel.SelectYesNo(tp,aux.Stringid(39454112,0)) then
		Duel.Hint(HINT_CARD,0,39454112)
		local dc={Duel.GetDiceResult()}
		local ac=1
		local ct=bit.band(ev,0xff)+bit.rshift(ev,16)
		if ct>1 then
			Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(39454112,1))
			local val,idx=Duel.AnnounceNumber(tp,table.unpack(dc,1,ct))
			ac=idx+1
		end
		if dc[ac]==1 or dc[ac]==3 or dc[ac]==5 then dc[ac]=6
		else dc[ac]=1 end
		Duel.SetDiceResult(table.unpack(dc))
		c39454112[0]=cid
	end
end
