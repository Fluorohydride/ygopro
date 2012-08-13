--魔法除去細菌兵器
function c54591086.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetLabel(0)
	e1:SetCost(c54591086.cost)
	e1:SetTarget(c54591086.target)
	e1:SetOperation(c54591086.activate)
	c:RegisterEffect(e1)
end
function c54591086.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(100)
	return true
end
function c54591086.rfilter(c)
	return not c:IsType(TYPE_TOKEN)
end
function c54591086.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()~=100 then return false end
		e:SetLabel(0)
		return Duel.CheckReleaseGroup(tp,c54591086.rfilter,1,nil)
	end
	local rg=Duel.SelectReleaseGroup(tp,c54591086.rfilter,1,5,nil)
	e:SetLabel(rg:GetCount())
	Duel.Release(rg,REASON_COST)
end
function c54591086.filter(c)
	return c:IsType(TYPE_SPELL) and c:IsAbleToGrave()
end
function c54591086.activate(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabel()
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(1-tp,c54591086.filter,1-tp,LOCATION_DECK,0,ct,ct,nil)
	if g:GetCount()>0 then
		Duel.SendtoGrave(g,REASON_EFFECT)
	end
end
