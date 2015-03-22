--コクーン・ヴェール
function c56641453.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c56641453.cost)
	e1:SetTarget(c56641453.target)
	e1:SetOperation(c56641453.activate)
	c:RegisterEffect(e1)
end
c56641453.list={[42682609]=17955766,[43751755]=43237273,[17363041]=54959865,
				[29246354]=17732278,[16241441]=89621922,[42239546]=80344569}
function c56641453.filter1(c,e,tp)
	if c:IsFacedown() then return false end
	local code=c:GetCode()
	local tcode=c56641453.list[code]
	return tcode and Duel.IsExistingTarget(c56641453.filter2,tp,0x13,0,1,nil,tcode,e,tp)
end
function c56641453.filter2(c,tcode,e,tp)
	return c:IsCode(tcode) and c:IsCanBeSpecialSummoned(e,0,tp,true,false) and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c56641453.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c56641453.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local res=e:GetLabel()==1
		e:SetLabel(0)
		return res and Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
			and Duel.CheckReleaseGroup(tp,c56641453.filter1,1,nil,e,tp) end
	e:SetLabel(0)
	local rg=Duel.SelectReleaseGroup(tp,c56641453.filter1,1,1,nil,e,tp)
	local code=rg:GetFirst():GetCode()
	local tcode=c56641453.list[code]
	Duel.Release(rg,REASON_COST)
	Duel.SetTargetParam(tcode)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,0x13)
end
function c56641453.activate(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CHANGE_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,0)
	e1:SetValue(c56641453.damval)
	e1:SetReset(RESET_PHASE+PHASE_END,1)
	Duel.RegisterEffect(e1,tp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local tcode=Duel.GetChainInfo(0,CHAININFO_TARGET_PARAM)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c56641453.filter2,tp,0x13,0,1,1,nil,tcode,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
function c56641453.damval(e,re,val,r,rp,rc)
	if bit.band(r,REASON_EFFECT)~=0 then return 0
	else return val end
end
