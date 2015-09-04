--閃光の宝札
function c48356796.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c48356796.accon)
	c:RegisterEffect(e1)
	--Effect Draw
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_DRAW_COUNT)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetRange(LOCATION_SZONE) 
	e2:SetTargetRange(1,0)
	e2:SetValue(2)
	e2:SetCondition(c48356796.drawcon)
	c:RegisterEffect(e2)
	--Effect Disable Field
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_DISABLE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_SZONE,0)
	e3:SetOperation(c48356796.disop)
	c:RegisterEffect(e3)
end
function c48356796.accon(e,tp,eg,ep,ev,re,r,rp)
	local c=Duel.GetLocationCount(tp,LOCATION_SZONE)
	if e:GetHandler():IsFacedown() then return c>0 end
	return c>1
end
function c48356796.filter(c)
	return c:IsCode(48356796) and c:IsFaceup()
end
function c48356796.drawcon(e)
	return Duel.IsExistingMatchingCard(c48356796.filter,e:GetHandlerPlayer(),LOCATION_SZONE,0,1,e:GetHandler())
end
function c48356796.disop(e,tp,eg,ep,ev,re,r,rp)
	return Duel.SelectDisableField(tp,1,LOCATION_SZONE,0,0)
end
