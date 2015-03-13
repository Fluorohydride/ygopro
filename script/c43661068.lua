--星に願いを
function c43661068.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c43661068.target)
	e1:SetOperation(c43661068.activate)
	c:RegisterEffect(e1)
end
function c43661068.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c43661068.tfilter(chkc,tp) end
	if chk==0 then return Duel.IsExistingTarget(c43661068.tfilter,tp,LOCATION_MZONE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c43661068.tfilter,tp,LOCATION_MZONE,0,1,1,nil,tp)
end
function c43661068.filter(c,atk,def)
	return c:IsFaceup() and c:GetLevel()>0 and (c:GetAttack()==atk or c:GetDefence()==def)
end
function c43661068.tfilter(c,tp)
	return c:IsFaceup() and c:GetLevel()>0
		and Duel.IsExistingMatchingCard(c43661068.filter,tp,LOCATION_MZONE,0,1,c,c:GetAttack(),c:GetDefence())
end
function c43661068.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local g=Duel.GetMatchingGroup(c43661068.filter,tp,LOCATION_MZONE,0,tc,tc:GetAttack(),tc:GetDefence())
		local lv=tc:GetLevel()
		local lc=g:GetFirst()
		while lc do
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CHANGE_LEVEL_FINAL)
			e1:SetValue(lv)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			lc:RegisterEffect(e1)
			lc=g:GetNext()
		end
	end
end
