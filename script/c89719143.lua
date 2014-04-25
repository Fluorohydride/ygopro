--決戦融合－ファイナル・フュージョン
function c89719143.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
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
	local bt=Duel.GetAttacker()
	local at=Duel.GetAttackTarget()
	return bt and bt:IsType(TYPE_FUSION) and at and at:IsType(TYPE_FUSION)
end
function c89719143.filter(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c89719143.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local bt=Duel.GetAttacker()
	local at=Duel.GetAttackTarget()
	if chkc then return chkc==bt or chkc==at end
	if chk==0 then return bt:IsOnField() and bt:IsCanBeEffectTarget(e) and at:IsOnField() and at:IsCanBeEffectTarget(e) end
	Duel.SetTargetCard(bt)
	Duel.SetTargetCard(at)
	local dam=bt:GetAttack()+at:GetAttack()
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,PLAYER_ALL,dam)
end
function c89719143.activate(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetChainInfo(0,CHAININFO_TARGET_PARAM)
	if Duel.NegateAttack() then
		Duel.Damage(1-tp,d,REASON_EFFECT)
		Duel.Damage(tp,d,REASON_EFFECT)
	end
end
