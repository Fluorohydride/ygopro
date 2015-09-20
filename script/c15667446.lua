--暗黒界の闘神 ラチナ
function c15667446.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(15667446,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c15667446.spcon)
	e1:SetTarget(c15667446.sptg)
	e1:SetOperation(c15667446.spop)
	c:RegisterEffect(e1)
end
function c15667446.spcon(e,tp,eg,ep,ev,re,r,rp)
	e:SetLabel(e:GetHandler():GetPreviousControler())
	return e:GetHandler():IsPreviousLocation(LOCATION_HAND) and bit.band(r,0x4040)==0x4040
end
function c15667446.atfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_FIEND)
end
function c15667446.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c15667446.atfilter(chkc) end
	if chk==0 then return true end
	if rp~=tp and tp==e:GetLabel() then
		e:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_ATKCHANGE)
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
		Duel.SelectTarget(tp,c15667446.atfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	else
		e:SetCategory(CATEGORY_SPECIAL_SUMMON)
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c15667446.spop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) and Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)>0 then
		local tc=Duel.GetFirstTarget()
		if tc and tc:IsFaceup() and tc:IsRelateToEffect(e) then
			Duel.BreakEffect()
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UPDATE_ATTACK)
			e1:SetValue(500)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e1)
		end
	end
end
