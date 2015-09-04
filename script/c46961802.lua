--クロス・アタック
function c46961802.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c46961802.target)
	e1:SetOperation(c46961802.activate)
	c:RegisterEffect(e1)
end
function c46961802.filter1(c,tp)
	return c:IsFaceup() and Duel.IsExistingTarget(c46961802.filter2,tp,LOCATION_MZONE,0,1,c,c:GetAttack())
end
function c46961802.filter2(c,atk)
	return c:IsFaceup() and c:GetAttack()==atk
end
function c46961802.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(c46961802.filter1,tp,LOCATION_MZONE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g1=Duel.SelectTarget(tp,c46961802.filter1,tp,LOCATION_MZONE,0,1,1,nil,tp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g2=Duel.SelectTarget(tp,c46961802.filter2,tp,LOCATION_MZONE,0,1,1,g1:GetFirst(),g1:GetFirst():GetAttack())
end
function c46961802.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc1=g:GetFirst()
	local tc2=g:GetNext()
	if tc1:IsFacedown() or tc2:IsFacedown() or not tc1:IsRelateToEffect(e) or not tc2:IsRelateToEffect(e)
		or tc1:GetAttack()~=tc2:GetAttack() then return end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DIRECT_ATTACK)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	tc1:RegisterEffect(e1)
	local e2=e1:Clone()
	tc2:RegisterEffect(e2)
	tc1:RegisterFlagEffect(46961802,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	tc2:RegisterFlagEffect(46961802,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	local e3=Effect.CreateEffect(e:GetHandler())
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_ATTACK_ANNOUNCE)
	e3:SetOperation(c46961802.atop)
	e3:SetLabelObject(tc2)
	e3:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	tc1:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetLabelObject(tc1)
	tc2:RegisterEffect(e4)
end
function c46961802.atop(e,tp,eg,ep,ev,re,r,rp)
	local oc=e:GetLabelObject()
	if oc:GetFlagEffect(46961802)~=0 then
		local e1=Effect.CreateEffect(e:GetOwner())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CANNOT_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		oc:RegisterEffect(e1,true)
	end
end
