--決戦融合－ファイナル・フュージョン
function c89719143.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_BATTLE_PHASE)
	e1:SetCondition(c89719143.condition)
	e1:SetTarget(c89719143.target)
	e1:SetOperation(c89719143.activate)
	c:RegisterEffect(e1)
end
function c89719143.condition(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local at=Duel.GetAttackTarget()
	return a and at and a:IsFaceup() and a:IsType(TYPE_FUSION) and at:IsFaceup() and at:IsType(TYPE_FUSION)
end
function c89719143.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local a=Duel.GetAttacker()
	local at=Duel.GetAttackTarget()
	if chkc then return chkc==a or chkc==at end
	if chk==0 then return a:IsOnField() and a:IsCanBeEffectTarget(e) and at:IsOnField() and at:IsCanBeEffectTarget(e) end
	Duel.SetTargetCard(a)
	Duel.SetTargetCard(at)
	local dam=a:GetAttack()+at:GetAttack()
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,PLAYER_ALL,dam)
end
function c89719143.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc1,tc2=Duel.GetFirstTarget()
	local dam=tc1:GetAttack()+tc2:GetAttack()
	if Duel.NegateAttack() then
		if tc1:IsRelateToEffect(e) and tc1:IsFaceup() and tc2:IsRelateToEffect(e) and tc2:IsFaceup() then 
			Duel.Damage(1-tp,dam,REASON_EFFECT)
			Duel.Damage(tp,dam,REASON_EFFECT)
		end
	end
end
