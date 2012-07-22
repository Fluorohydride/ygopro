--バトル・ブレイク
function c22047978.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c22047978.condition)
	e1:SetTarget(c22047978.target)
	e1:SetOperation(c22047978.activate)
	c:RegisterEffect(e1)
end
function c22047978.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp~=Duel.GetTurnPlayer()
end
function c22047978.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local tg=Duel.GetAttacker()
	if chkc then return chkc==tg end
	if chk==0 then return tg:IsOnField() and tg:IsDestructable() and tg:IsCanBeEffectTarget(e) end
	Duel.SetTargetCard(tg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,tg,1,0,0)
end
function c22047978.cfilter(c)
	return c:IsType(TYPE_MONSTER) and not c:IsPublic()
end
function c22047978.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsAttackable() and not tc:IsStatus(STATUS_ATTACK_CANCELED) then
		local g=Duel.GetMatchingGroup(c22047978.cfilter,1-tp,LOCATION_HAND,0,nil)
		if g:GetCount()>0 and Duel.SelectYesNo(1-tp,aux.Stringid(22047978,0)) then
			Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_CONFIRM)
			local cg=g:Select(1-tp,1,1,nil)
			Duel.ConfirmCards(tp,cg)
			Duel.ShuffleHand(1-tp)
			return
		end
		if Duel.Destroy(tc,REASON_EFFECT)>0 then
			Duel.SkipPhase(1-tp,PHASE_BATTLE,RESET_PHASE+PHASE_BATTLE,1)
		end
	end
end
