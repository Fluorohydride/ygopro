--揺れる発条秤
function c42548470.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c42548470.target)
	e1:SetOperation(c42548470.activate)
	c:RegisterEffect(e1)
end
function c42548470.filter1(c,tp)
	local lv1=c:GetLevel()
	return c:IsFaceup() and c:IsSetCard(0x58) and lv1~=0 and Duel.IsExistingTarget(c42548470.filter2,tp,LOCATION_MZONE,0,1,c,lv1)
end
function c42548470.filter2(c,lv)
	local lv2=c:GetLevel()
	return c:IsFaceup() and c:IsSetCard(0x58) and lv2~=0 and lv2~=lv
end
function c42548470.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(c42548470.filter1,tp,LOCATION_MZONE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g1=Duel.SelectTarget(tp,c42548470.filter1,tp,LOCATION_MZONE,0,1,1,nil,tp)
	local tc1=g1:GetFirst()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g2=Duel.SelectTarget(tp,c42548470.filter2,tp,LOCATION_MZONE,0,1,1,tc1,tc1:GetLevel())
end
function c42548470.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc1=g:GetFirst()
	local tc2=g:GetNext()
	local lv1=tc1:GetLevel()
	local lv2=tc2:GetLevel()
	if lv1==lv2 then return end
	if tc1:IsFaceup() and tc1:IsRelateToEffect(e) and tc2:IsFaceup() and tc2:IsRelateToEffect(e) then
		Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TARGET)
		local sg=g:Select(1-tp,1,1,nil)
		if sg:GetFirst()==tc1 then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CHANGE_LEVEL)
			e1:SetValue(lv1)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			tc2:RegisterEffect(e1)
			if lv1<lv2 and Duel.IsPlayerCanDraw(tp,1) and Duel.SelectYesNo(tp,aux.Stringid(42548470,0)) then Duel.Draw(tp,1,REASON_EFFECT) end
		else
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CHANGE_LEVEL)
			e1:SetValue(lv2)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			tc1:RegisterEffect(e1)
			if lv2<lv1 and Duel.IsPlayerCanDraw(tp,1) and Duel.SelectYesNo(tp,aux.Stringid(42548470,0)) then Duel.Draw(tp,1,REASON_EFFECT) end
		end
	end
end
