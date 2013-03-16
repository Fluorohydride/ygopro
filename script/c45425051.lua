--妖精王オベロン
function c45425051.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetCondition(c45425051.con)
	e1:SetTarget(c45425051.tg)
	e1:SetValue(500)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	e2:SetValue(500)
	c:RegisterEffect(e2)
end
function c45425051.con(e)
	return e:GetHandler():IsDefencePos()
end
function c45425051.tg(e,c)
	return c:IsRace(RACE_PLANT)
end
