--体力増強剤スーパーZ
function c29389368.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_RECOVER)
	e1:SetCode(EVENT_PRE_DAMAGE_CALCULATE)
	e1:SetCondition(c29389368.condition)
	e1:SetTarget(c29389368.target)
	e1:SetOperation(c29389368.activate)
	c:RegisterEffect(e1)
end
function c29389368.condition(e,tp,eg,ep,ev,re,r,rp)
	local ar=Duel.GetAttacker()
	local at=Duel.GetAttackTarget()
	local v1=0
	if ar:IsPosition(POS_DEFENCE) and ar:IsHasEffect(EFFECT_DEFENCE_ATTACK) then v1=ar:GetDefence()
	else v1=ar:GetAttack() end
	if at then
		if at:IsPosition(POS_ATTACK) then
			return v1-at:GetAttack()>=2000
		elseif ar:IsHasEffect(EFFECT_PIERCE)
			return v1-at:GetDefence()>=2000
		else
			return false
		end
	else
		return v1>=2000
	end
end
function c29389368.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,4000)
end
function c29389368.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Recover(tp,4000,REASON_EFFECT)
end
