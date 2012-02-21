--ミニマム・ガッツ
function c99004752.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c99004752.cost)
	e1:SetTarget(c99004752.target)
	e1:SetOperation(c99004752.activate)
	c:RegisterEffect(e1)
end
function c99004752.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,nil,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,nil,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c99004752.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and chkc:IsFaceup() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsFaceup,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,Card.IsFaceup,tp,0,LOCATION_MZONE,1,1,nil)
end
function c99004752.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_ATTACK_FINAL)
		e1:SetValue(0)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_BATTLE_DESTROYED)
		e2:SetCondition(c99004752.damcon)
		e2:SetOperation(c99004752.damop)
		e2:SetReset(RESET_EVENT+0x17a0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e2)
	end
end
function c99004752.damcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:GetPreviousControler()==c:GetControler()
end
function c99004752.damop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local atk=c:GetAttack()
	if atk<0 then atk=0 end
	if atk>0 then
		Duel.Hint(HINT_CARD,tp,99004752)
		Duel.Hint(HINT_CARD,1-tp,99004752)
		Duel.Damage(tp,atk,REASON_EFFECT)
	end
end
