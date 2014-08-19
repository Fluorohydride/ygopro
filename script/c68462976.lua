--魔法族の里
function c68462976.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--adjust
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_ADJUST)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c68462976.adjustop)
	c:RegisterEffect(e2)
	--cannot activate
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_CANNOT_ACTIVATE)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(1,1)
	e3:SetLabel(0)
	e3:SetValue(c68462976.actlimit)
	c:RegisterEffect(e3)
	e2:SetLabelObject(e3)
end
function c68462976.actlimit(e,te,tp)
	if not te:IsHasType(EFFECT_TYPE_ACTIVATE) or not te:IsActiveType(TYPE_SPELL) then return false end
	if tp==e:GetHandlerPlayer() then return e:GetLabel()==1
	else return e:GetLabel()==2 end
end
function c68462976.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_SPELLCASTER)
end
function c68462976.adjustop(e,tp,eg,ep,ev,re,r,rp)
	local b1=Duel.IsExistingMatchingCard(c68462976.filter,tp,LOCATION_MZONE,0,1,nil)
	local b2=Duel.IsExistingMatchingCard(c68462976.filter,tp,0,LOCATION_MZONE,1,nil)
	local te=e:GetLabelObject()
	if not b1 then te:SetLabel(1)
	elseif b1 and not b2 then te:SetLabel(2)
	else te:SetLabel(0) end
end
