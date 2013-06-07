--ヴァリュアブル・フォーム
function c64038662.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c64038662.eftg1)
	e1:SetOperation(c64038662.efop)
	c:RegisterEffect(e1)
	--equip or spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetDescription(aux.Stringid(64038662,0))
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetTarget(c64038662.eftg2)
	e2:SetOperation(c64038662.efop)
	c:RegisterEffect(e2)
end
function c64038662.filter1(c)
	return c:IsFaceup() and c:IsSetCard(0x56)
end
function c64038662.filter2(c,e,tp)
	return c:IsFaceup() and c:IsSetCard(0x56) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c64038662.select(e,tp,b1,b2)
	local op=0
	if b1 and b2 then
		op=Duel.SelectOption(tp,aux.Stringid(64038662,2),aux.Stringid(64038662,3))+1
	elseif b1 then
		op=Duel.SelectOption(tp,aux.Stringid(64038662,2))+1
	else op=Duel.SelectOption(tp,aux.Stringid(64038662,3))+2 end
	e:SetLabel(op)
	if op==1 then
		e:SetCategory(CATEGORY_EQUIP)
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(64038662,4))
		local g1=Duel.SelectTarget(tp,c64038662.filter1,tp,LOCATION_MZONE,0,1,1,nil)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
		local g2=Duel.SelectTarget(tp,c64038662.filter1,tp,LOCATION_MZONE,0,1,1,g1:GetFirst())
		e:SetLabelObject(g1:GetFirst())
	else
		e:SetCategory(CATEGORY_SPECIAL_SUMMON)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectTarget(tp,c64038662.filter2,tp,LOCATION_SZONE,0,1,1,nil,e,tp)
		Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
	end
	e:GetHandler():RegisterFlagEffect(64038662,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c64038662.eftg1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then
		if e:GetLabel()==0 or e:GetLabel()==1 then return false
		else return chkc:IsLocation(LOCATION_SZONE) and chkc:IsControler(tp) and c64038662.filter2(chkc,e,tp) end
	end
	if chk==0 then return true end
	local b1=Duel.IsExistingTarget(c64038662.filter1,tp,LOCATION_MZONE,0,2,nil) and Duel.GetLocationCount(tp,LOCATION_SZONE)>0
	local b2=Duel.IsExistingTarget(c64038662.filter2,tp,LOCATION_SZONE,0,1,nil,e,tp) and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
	if (not b1 and not b2) or not Duel.SelectYesNo(tp,aux.Stringid(64038662,1)) then
		e:SetProperty(0)
		e:SetCategory(0)
		e:SetLabel(0)
		return
	end
	e:SetProperty(EFFECT_FLAG_CARD_TARGET)
	c64038662.select(e,tp,b1,b2)
end
function c64038662.eftg2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then
		if e:GetLabel()==0 or e:GetLabel()==1 then return false
		else return chkc:IsLocation(LOCATION_SZONE) and chkc:IsControler(tp) and c64038662.filter2(chkc,e,tp) end
	end
	local b1=Duel.IsExistingTarget(c64038662.filter1,tp,LOCATION_MZONE,0,2,nil) and Duel.GetLocationCount(tp,LOCATION_SZONE)>0
	local b2=Duel.IsExistingTarget(c64038662.filter2,tp,LOCATION_SZONE,0,1,nil,e,tp) and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
	if chk==0 then return e:GetHandler():GetFlagEffect(64038662)==0 and (b1 or b2) end
	c64038662.select(e,tp,b1,b2)
end
function c64038662.efop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if e:GetLabel()==0 then return
	elseif e:GetLabel()==1 then
		local tc1=e:GetLabelObject()
		local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
		local tc2=g:GetFirst()
		if tc1==tc2 then tc2=g:GetNext() end
		if tc1:IsFaceup() and tc2:IsFaceup() and tc1:IsRelateToEffect(e) and tc2:IsRelateToEffect(e) and Duel.Equip(tp,tc1,tc2,false) then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_EQUIP_LIMIT)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			e1:SetValue(c64038662.eqlimit)
			e1:SetLabelObject(tc2)
			tc1:RegisterEffect(e1)
		end
	else
		local tc=Duel.GetFirstTarget()
		if tc and tc:IsRelateToEffect(e) then
			Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
		end
	end
end
function c64038662.eqlimit(e,c)
	return c==e:GetLabelObject()
end
