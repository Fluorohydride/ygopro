--暴風雨
function c45653036.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetHintTiming(0,0x1e0)
	e1:SetTarget(c45653036.target)
	e1:SetOperation(c45653036.activate)
	c:RegisterEffect(e1)
end
function c45653036.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x18) and c:IsAttackAbove(1000)
end
function c45653036.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c45653036.cfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c45653036.cfilter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c45653036.cfilter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c45653036.filter1(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c45653036.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local atk=tc:GetAttack()
		local g1=Duel.GetMatchingGroup(c45653036.filter1,tp,0,LOCATION_ONFIELD,nil)
		local g2=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
		local opt=0
		local b1=atk>=1000 and g1:GetCount()>0
		local b2=atk>=2000 and g2:GetCount()>1
		if b1 and b2 then opt=Duel.SelectOption(tp,aux.Stringid(45653036,0),aux.Stringid(45653036,1))
		elseif b1 then opt=Duel.SelectOption(tp,aux.Stringid(45653036,0))
		elseif b2 then opt=Duel.SelectOption(tp,aux.Stringid(45653036,1))+1
		else opt=2 end
		if opt==0 then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UPDATE_ATTACK)
			e1:SetValue(-1000)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e1)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
			local dg=g1:Select(tp,1,1,nil)
			Duel.Destroy(dg,REASON_EFFECT)
		elseif opt==1 then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UPDATE_ATTACK)
			e1:SetValue(-2000)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e1)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
			local dg=g2:Select(tp,2,2,nil)
			Duel.Destroy(dg,REASON_EFFECT)
		end
	end
end
