--BK シャドー
function c35537251.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(35537251,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_HAND)
	e1:SetCost(c35537251.spcost)
	e1:SetTarget(c35537251.sptg)
	e1:SetOperation(c35537251.spop)
	c:RegisterEffect(e1)
end
function c35537251.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	local g=Group.CreateGroup()
	for i=0,4 do
		local tc=Duel.GetFieldCard(tp,LOCATION_MZONE,i)
		if tc and tc:IsFaceup() and tc:IsSetCard(0x84) and tc:IsType(TYPE_XYZ) then
			g:Merge(tc:GetOverlayGroup())
		end
	end
	if chk==0 then return g:GetCount()>0 and Duel.GetFlagEffect(tp,35537251)==0 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVEXYZ)
	local sg=g:Select(tp,1,1,nil)
	Duel.SendtoGrave(sg,REASON_COST)
	Duel.RegisterFlagEffect(tp,35537251,RESET_PHASE+PHASE_END,0,1)
end
function c35537251.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c35537251.spop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
