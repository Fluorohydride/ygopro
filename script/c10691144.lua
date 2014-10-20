--氷結界の鏡
function c10691144.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetOperation(c10691144.activate)
	c:RegisterEffect(e1)
end
function c10691144.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFlagEffect(tp,10691144)~=0 then return end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetProperty(EFFECT_FLAG_DELAY)
	e1:SetCode(EVENT_REMOVE)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetCondition(c10691144.rmcon)
	e1:SetOperation(c10691144.rmop)
	Duel.RegisterEffect(e1,tp)
	Duel.RegisterFlagEffect(tp,10691144,RESET_PHASE+PHASE_END,0,1)
end
function c10691144.rmcon(e,tp,eg,ep,ev,re,r,rp)
	local flag=0
	local tc=eg:GetFirst()
	while tc do
		local ploc=tc:GetPreviousLocation()
		if tc:IsReason(REASON_EFFECT) and not tc:IsReason(REASON_REDIRECT) and bit.band(ploc,0x1e)~=0
			and tc:GetPreviousControler()==tp and tc:GetReasonEffect():GetOwner():IsType(TYPE_MONSTER) then
			flag=bit.bor(flag,ploc)
		end
		tc=eg:GetNext()
	end
	e:SetLabel(flag)
	return flag~=0
end
function c10691144.rmop(e,tp,eg,ep,ev,re,r,rp)
	local g=Group.CreateGroup()
	local flag=e:GetLabel()
	if bit.band(flag,LOCATION_HAND)~=0 then
		local rg=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,0,LOCATION_HAND,nil)
		if rg:GetCount()>0 then
			local ct=1
			if rg:GetCount()>1 then ct=Duel.SelectOption(tp,aux.Stringid(10691144,3),aux.Stringid(10691144,4))+1 end
			g:Merge(rg:RandomSelect(tp,ct))
		end
	end
	if bit.band(flag,LOCATION_ONFIELD)~=0 then
		local rg=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,0,LOCATION_ONFIELD,nil)
		if rg:GetCount()>0 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
			g:Merge(rg:Select(tp,1,2,nil))
		end
	end
	if bit.band(flag,LOCATION_GRAVE)~=0 then
		local rg=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,0,LOCATION_GRAVE,nil)
		if rg:GetCount()>0 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
			g:Merge(rg:Select(tp,1,2,nil))
		end
	end
	Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
end
