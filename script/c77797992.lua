--森の聖獣 アルパカリブ
function c77797992.initial_effect(c)
	--indes1
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetCondition(c77797992.cona)
	e1:SetTarget(c77797992.targeta)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	--cannot be target
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetCondition(c77797992.cond)
	e2:SetTarget(c77797992.targetd)
	e2:SetValue(aux.tgval)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	c:RegisterEffect(e3)
end
function c77797992.cona(e)
	return e:GetHandler():IsAttackPos()
end
function c77797992.targeta(e,c)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:IsRace(RACE_WINDBEAST+RACE_PLANT+RACE_INSECT)
end
function c77797992.cond(e)
	return e:GetHandler():IsDefencePos()
end
function c77797992.targetd(e,c)
	return c:IsPosition(POS_FACEUP_DEFENCE) and c:IsRace(RACE_WINDBEAST+RACE_PLANT+RACE_INSECT)
end
