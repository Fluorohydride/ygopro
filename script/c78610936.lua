--エクシーズ・オーバーディレイ
function c78610936.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetTarget(c78610936.target)
	e1:SetOperation(c78610936.activate)
	c:RegisterEffect(e1)
end
function c78610936.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ) and c:IsAbleToExtra() and c:GetOverlayCount()>0
end
function c78610936.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsLocation(LOCATION_MZONE) and c78610936.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c78610936.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c78610936.filter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
	Duel.SetChainLimit(aux.FALSE)
end
function c78610936.spfilter(c,e,tp)
	return c:IsLocation(LOCATION_GRAVE) and c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP,1-tp) and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c78610936.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if not tc:IsRelateToEffect(e) then return end
	local mg=tc:GetOverlayGroup()
	Duel.SendtoGrave(mg,REASON_EFFECT)
	if Duel.SendtoDeck(tc,nil,0,REASON_EFFECT)>0 then
		local g=mg:Filter(c78610936.spfilter,nil,e,tp)
		local ft=Duel.GetLocationCount(1-tp,LOCATION_MZONE)
		if ft>0 and g:GetCount()>0 then
			if g:GetCount()>ft then
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
				g=g:Select(tp,ft,ft,nil)
			end
			local tc=g:GetFirst()
			while tc do
				Duel.SpecialSummonStep(tc,0,tp,1-tp,false,false,POS_FACEUP_DEFENCE)
				if tc:GetLevel()>0 then
					local e1=Effect.CreateEffect(e:GetHandler())
					e1:SetType(EFFECT_TYPE_SINGLE)
					e1:SetCode(EFFECT_UPDATE_LEVEL)
					e1:SetValue(-1)
					e1:SetReset(RESET_EVENT+0x1fe0000)
					tc:RegisterEffect(e1)
					tc=g:GetNext()
				end
			end
			Duel.SpecialSummonComplete()
		end
	end
end
