--発条の巻き上げ
function c83319610.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c83319610.mttg1)
	e1:SetOperation(c83319610.mtop)
	c:RegisterEffect(e1)
	--material
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(83319610,0))
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetCost(c83319610.mtcost)
	e2:SetTarget(c83319610.mttg2)
	e2:SetOperation(c83319610.mtop)
	e2:SetLabel(1)
	c:RegisterEffect(e2)
	--spsummon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(83319610,1))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_QUICK_O)
	e3:SetRange(LOCATION_SZONE)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetCode(EVENT_FREE_CHAIN)
	e3:SetCost(c83319610.spcost)
	e3:SetTarget(c83319610.sptg)
	e3:SetOperation(c83319610.spop)
	c:RegisterEffect(e3)
end
function c83319610.filter1(c,e,tp)
	return c:IsFaceup() and c:IsType(TYPE_XYZ) and c:IsRace(RACE_MACHINE)
		and Duel.IsExistingMatchingCard(c83319610.filter2,tp,LOCATION_HAND+LOCATION_MZONE,0,1,c,e)
end
function c83319610.filter2(c,e)
	return c:IsSetCard(0x58) and c:IsType(TYPE_MONSTER) and (c:IsLocation(LOCATION_HAND) or c:IsFaceup()) and not c:IsImmuneToEffect(e)
end
function c83319610.mttg1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and c83319610.filter1(chkc,e,tp) end
	if chk==0 then return true end
	if Duel.IsExistingTarget(c83319610.filter1,tp,LOCATION_MZONE,0,1,nil,e,tp)
		and Duel.SelectYesNo(tp,aux.Stringid(83319610,2)) then
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
		e:GetHandler():RegisterFlagEffect(83319610,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
		Duel.SelectTarget(tp,c83319610.filter1,tp,LOCATION_MZONE,0,1,1,nil,e,tp)
		e:SetLabel(1)
	else
		e:SetProperty(0)
		e:SetLabel(0)
	end
end
function c83319610.mtcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(83319610)==0 end
	e:GetHandler():RegisterFlagEffect(83319610,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c83319610.mttg2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and c83319610.filter1(chkc,e,tp) end
	if chk==0 then return Duel.IsExistingTarget(c83319610.filter1,tp,LOCATION_MZONE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c83319610.filter1,tp,LOCATION_MZONE,0,1,1,nil,e,tp)
end
function c83319610.mtop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if not tc:IsRelateToEffect(e) or tc:IsImmuneToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
	local g=Duel.SelectMatchingCard(tp,c83319610.filter2,tp,LOCATION_HAND+LOCATION_MZONE,0,1,1,tc,e)
	if g:GetCount()>0 then
		Duel.Overlay(tc,g)
	end
end
function c83319610.spfilter1(c,e,tp)
	return c:IsFaceup() and c:IsSetCard(0x58) and c:IsType(TYPE_XYZ)
		and Duel.IsExistingMatchingCard(c83319610.spfilter2,tp,LOCATION_EXTRA,0,1,nil,c:GetRank()+1,e,tp)
end
function c83319610.spfilter2(c,rk,e,tp)
	return c:GetRank()==rk and c:IsSetCard(0x58)
		and c:IsCanBeSpecialSummoned(e,SUMMON_TYPE_XYZ,tp,false,false)
end
function c83319610.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c83319610.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and c83319610.spfilter1(chkc,e,tp) end
	if chk==0 then return not e:GetHandler():IsStatus(STATUS_CHAINING)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingTarget(c83319610.spfilter1,tp,LOCATION_MZONE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c83319610.spfilter1,tp,LOCATION_MZONE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c83319610.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<0 then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsFacedown() or not tc:IsRelateToEffect(e) or tc:IsControler(1-tp) or tc:IsImmuneToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c83319610.spfilter2,tp,LOCATION_EXTRA,0,1,1,nil,tc:GetRank()+1,e,tp)
	local sc=g:GetFirst()
	if sc then
		local mg=tc:GetOverlayGroup()
		if mg:GetCount()~=0 then
			Duel.Overlay(sc,mg)
		end
		sc:SetMaterial(Group.FromCards(tc))
		Duel.Overlay(sc,Group.FromCards(tc))
		Duel.SpecialSummon(sc,SUMMON_TYPE_XYZ,tp,tp,false,false,POS_FACEUP)
		sc:CompleteProcedure()
	end
end
