--N・グロー・モス
function c17732278.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(17732278,0))
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c17732278.condition)
	e1:SetTarget(c17732278.target)
	e1:SetOperation(c17732278.activate)
	c:RegisterEffect(e1)
end
function c17732278.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler()==Duel.GetAttacker() or e:GetHandler()==Duel.GetAttackTarget()
end
function c17732278.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(17732278)==0 end
	e:GetHandler():RegisterFlagEffect(17732278,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_DAMAGE,0,1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,1-tp,1)
end
function c17732278.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if Duel.Draw(1-tp,1,REASON_EFFECT)==0 then return end
	local tc=Duel.GetOperatedGroup():GetFirst()
	Duel.ConfirmCards(tp,tc)
	if tc:IsType(TYPE_MONSTER) then
		Duel.SkipPhase(Duel.GetTurnPlayer(),PHASE_BATTLE,RESET_PHASE+PHASE_BATTLE,1)
	elseif tc:IsType(TYPE_SPELL) then
		if c==Duel.GetAttacker() and not c:IsHasEffect(EFFECT_CANNOT_DIRECT_ATTACK)
			and c:IsRelateToEffect(e) and c:IsFaceup() and Duel.SelectYesNo(tp,aux.Stringid(17732278,1)) then
			Duel.ChangeAttackTarget(nil)
		end
	else
		if c:IsRelateToEffect(e) and c:IsFaceup() then
			Duel.ChangePosition(c,POS_FACEUP_DEFENCE)
		end
	end
	Duel.ShuffleHand(1-tp)
end
