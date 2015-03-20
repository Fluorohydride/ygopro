--D・マグネンU
function c29947751.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_SET_AVAILABLE+EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetCondition(c29947751.cona)
	e1:SetTarget(c29947751.tga)
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
	e3:SetTargetRange(LOCATION_MZONE,0)
	e3:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e3:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e3:SetCondition(c29947751.cond)
	e3:SetTarget(c29947751.atlimit)
	e3:SetValue(aux.imval1)
	c:RegisterEffect(e3)
end
function c29947751.cona(e)
	return e:GetHandler():IsAttackPos()
		and Duel.IsExistingMatchingCard(Card.IsFaceup,e:GetHandler():GetControler(),0,LOCATION_MZONE,1,nil)
end
function c29947751.filter(c,atk)
	return c:IsFaceup() and c:GetAttack()>atk
end
function c29947751.tga(e,c)
	if c:IsFaceup() then
		return Duel.IsExistingMatchingCard(c29947751.filter,c:GetControler(),LOCATION_MZONE,0,1,c,c:GetAttack())
	else return true end
end
function c29947751.vala(e,c)
	return c==e:GetHandler()
end
function c29947751.cond(e)
	return e:GetHandler():IsDefencePos()
end
function c29947751.atlimit(e,c)
	return c~=e:GetHandler()
end
