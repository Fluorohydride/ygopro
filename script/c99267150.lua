--F・G・D
function c99267150.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcFunRep(c,aux.FilterBoolFunction(Card.IsRace,RACE_DRAGON),5,true)
	--battle indestructable
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e2:SetValue(c99267150.batfilter)
	c:RegisterEffect(e2)
	--spsummon condition
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e3:SetCode(EFFECT_SPSUMMON_CONDITION)
	e3:SetValue(aux.fuslimit)
	c:RegisterEffect(e3)
end
function c99267150.batfilter(e,c)
	return c:IsAttribute(0x2f)
end
function c99267150.check_fusion_material_48144509(m,chkf)
	local g1=m:Filter(Card.IsRace,nil,RACE_DRAGON)
	if chkf~=PLAYER_NONE and not g1:IsExists(Card.IsOnField,1,nil) then return false end
	local ct1=g1:GetCount()
	local ct2=g1:FilterCount(Card.IsLocation,nil,LOCATION_EXTRA)
	return (ct1-ct2)+math.min(ct2,2)>=5
end
function c99267150.select_fusion_material_48144509(tp,m,chkf)
	local cc=5
	local mg1=m:Filter(Card.IsRace,nil,RACE_DRAGON)
	local mg2=mg1:Filter(Card.IsLocation,nil,LOCATION_EXTRA)
	local sg=Group.CreateGroup()
	if mg2:GetCount()>2 then
		mg1:Sub(mg2)
		if mg1:GetCount()<5 or Duel.SelectYesNo(tp,aux.Stringid(48144509,0)) then
			local minc=1
			if mg1:GetCount()<4 then minc=2 end
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			local g=mg2:Select(tp,minc,2,nil)
			sg:Merge(g)
			cc=cc-g:GetCount()
		end
	end
	if chkf~=PLAYER_NONE and mg1:GetCount()>cc then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		local g=mg1:FilterSelect(tp,aux.FConditionCheckF,1,1,nil,chkf)
		mg1:Sub(g)
		sg:Merge(g)
		cc=cc-1
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	local g=mg1:Select(tp,cc,cc,nil)
	sg:Merge(g)
	Duel.SetFusionMaterial(sg)
	return sg
end
