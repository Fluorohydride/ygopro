--D・マグネンU
function c29947751.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_SELECT_BATTLE_TARGET)
	e1:SetCondition(c29947751.cona)
	e1:SetValue(c29947751.vala)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_DIRECT_ATTACK)
	e2:SetCondition(c29947751.cona)
	c:RegisterEffect(e2)
	--def
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_MZONE)
	e3:SetTargetRange(0,LOCATION_MZONE)
	e3:SetCode(EFFECT_CANNOT_SELECT_BATTLE_TARGET)
	e3:SetCondition(c29947751.cond)
	e3:SetValue(c29947751.atlimit)
	c:RegisterEffect(e3)
end
function c29947751.cona(e)
	return e:GetHandler():IsAttackPos()
		and Duel.IsExistingMatchingCard(Card.IsFaceup,e:GetHandler():GetControler(),0,LOCATION_MZONE,1,nil)
end
function c29947751.filter(c,atk)
	return c:IsFaceup() and c:GetAttack()>atk
end
function c29947751.vala(e,c)
	if c:IsFaceup() then
		return Duel.IsExistingMatchingCard(c29947751.filter,c:GetControler(),LOCATION_MZONE,0,1,c,c:GetAttack())
	else return true end
end
function c29947751.cond(e)
	return e:GetHandler():IsDefencePos()
end
function c29947751.atlimit(e,c)
	return c~=e:GetHandler()
end
