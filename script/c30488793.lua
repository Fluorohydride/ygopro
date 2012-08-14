--反発力
function c30488793.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetCode(EVENT_ATTACK_DISABLED)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCondition(c30488793.condition)
	e1:SetTarget(c30488793.target)
	e1:SetOperation(c30488793.activate)
	c:RegisterEffect(e1)
end
function c30488793.condition(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local t=Duel.GetAttackTarget()
	return a:IsLocation(LOCATION_MZONE) and t and t:IsLocation(LOCATION_MZONE) and t:IsPosition(POS_FACEUP_ATTACK)
end
function c30488793.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local a1=Duel.GetAttacker():GetAttack()
	local a2=Duel.GetAttackTarget():GetAttack()
	local dam=a1-a2
	if dam<0 then dam=-dam end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(dam)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,0,0,1-tp,dam)
end
function c30488793.activate(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
