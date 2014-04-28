--暗黒界の鬼神 ケルト
function c34968834.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(34968834,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c34968834.spcon)
	e1:SetTarget(c34968834.sptg)
	e1:SetOperation(c34968834.spop)
	c:RegisterEffect(e1)
end
function c34968834.spcon(e,tp,eg,ep,ev,re,r,rp)
	e:SetLabel(e:GetHandler():GetPreviousControler())
	return e:GetHandler():IsPreviousLocation(LOCATION_HAND) and bit.band(r,0x4040)==0x4040
end
function c34968834.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c34968834.filter(c,e,tp)
	return c:IsRace(RACE_FIEND)
		and ((Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and c:IsCanBeSpecialSummoned(e,0,tp,false,false))
		or (Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0 and c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP,1-tp)))
end
function c34968834.spop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)==0 then return end
	if rp~=tp and tp==e:GetLabel() and Duel.IsExistingMatchingCard(c34968834.filter,tp,LOCATION_DECK,0,1,nil,e,tp)
		and Duel.SelectYesNo(tp,aux.Stringid(34968834,1)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(tp,c34968834.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
		local tc=g:GetFirst()
		if tc then
			local b1=Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and tc:IsCanBeSpecialSummoned(e,0,tp,false,false)
			local b2=Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0 and tc:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP,1-tp)
			local op=0
			if b1 and b2 then
				op=Duel.SelectOption(tp,aux.Stringid(34968834,2),aux.Stringid(34968834,3))
			elseif b1 then
				op=Duel.SelectOption(tp,aux.Stringid(34968834,2))
			elseif b2 then
				op=Duel.SelectOption(tp,aux.Stringid(34968834,3))+1
			else return end
			if op==0 then
				Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
			else
				Duel.SpecialSummon(tc,0,tp,1-tp,false,false,POS_FACEUP)
			end
		end
	end
end
