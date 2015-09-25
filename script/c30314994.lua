--エレメント・ドラゴン
function c30314994.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(500)
	e1:SetCondition(c30314994.atkcon)
	c:RegisterEffect(e1)
	--chain attack
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(30314994,0))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_BATTLED)
	e2:SetCondition(c30314994.atcon)
	e2:SetOperation(c30314994.atop)
	c:RegisterEffect(e2)
end
function c30314994.filter(c,att)
	return c:IsFaceup() and c:IsAttribute(att)
end
function c30314994.atkcon(e)
	return Duel.IsExistingMatchingCard(c30314994.filter,0,LOCATION_MZONE,LOCATION_MZONE,1,nil,ATTRIBUTE_FIRE)
end
function c30314994.atcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return bc and bc:IsStatus(STATUS_BATTLE_DESTROYED) and c:IsChainAttackable() and c:IsStatus(STATUS_OPPO_BATTLE)
		and not c:IsDisabled() and Duel.IsExistingMatchingCard(c30314994.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,ATTRIBUTE_WIND)
end
function c30314994.atop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChainAttack()
end
