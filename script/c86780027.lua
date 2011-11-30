--一族の結束
function c86780027.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--atk up
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetLabel(0)
	e2:SetTarget(c86780027.tg)
	e2:SetValue(800)
	c:RegisterEffect(e2)
	--adjust
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_ADJUST)
	e3:SetRange(LOCATION_SZONE)
	e3:SetOperation(c86780027.adjustop)
	c:RegisterEffect(e3)
	e3:SetLabelObject(e2)
end
function c86780027.tg(e,c)
	return c:IsRace(e:GetLabel())
end
function c86780027.adjustop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsType,tp,LOCATION_GRAVE,0,nil,TYPE_MONSTER)
	local te=e:GetLabelObject()
	if g:GetCount()==0 then te:SetLabel(0)
	else
		local ct=g:GetClassCount(Card.GetOriginalRace)
		if ct==1 then te:SetLabel(g:GetFirst():GetOriginalRace())
		else te:SetLabel(0) end
	end
end
