--No.93 希望皇ホープ・カイザー
function c23187256.initial_effect(c)
	--xyz summon
	c:EnableReviveLimit()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetCondition(c23187256.xyzcon)
	e1:SetOperation(c23187256.xyzop)
	e1:SetValue(SUMMON_TYPE_XYZ)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(23187256,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetTarget(c23187256.target)
	e2:SetOperation(c23187256.operation)
	c:RegisterEffect(e2)
	--indestructable
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e3:SetCondition(c23187256.indcon)
	e3:SetValue(1)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	c:RegisterEffect(e4)
end
c23187256.xyz_number=93
function c23187256.mfilter(c,xyzc)
	return c:IsFaceup() and c:IsType(TYPE_XYZ) and c:IsSetCard(0x48) and c:GetOverlayCount()>0 and c:IsCanBeXyzMaterial(xyzc)
end
function c23187256.xyzfilter1(c,g,ct)
	return g:IsExists(c23187256.xyzfilter2,ct,c,c:GetRank())
end
function c23187256.xyzfilter2(c,rk)
	return c:GetRank()==rk
end
function c23187256.xyzcon(e,c,og)
	if c==nil then return true end
	local tp=c:GetControler()
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local ct=math.max(1,-ft)
	local mg=Duel.GetMatchingGroup(c23187256.mfilter,tp,LOCATION_MZONE,0,nil,c)
	return mg:IsExists(c23187256.xyzfilter1,1,nil,mg,ct)
end
function c23187256.xyzop(e,tp,eg,ep,ev,re,r,rp,c,og)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local ct=math.max(1,-ft)
	local mg=Duel.GetMatchingGroup(c23187256.mfilter,tp,LOCATION_MZONE,0,nil,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
	local g1=mg:FilterSelect(tp,c23187256.xyzfilter1,1,1,nil,mg,ct)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
	local g2=mg:FilterSelect(tp,c23187256.xyzfilter2,ct,63,g1:GetFirst(),g1:GetFirst():GetRank())
	g1:Merge(g2)
	local sg=Group.CreateGroup()
	local tc=g1:GetFirst()
	while tc do
		sg:Merge(tc:GetOverlayGroup())
		tc=g1:GetNext()
	end
	Duel.SendtoGrave(sg,REASON_RULE)
	c:SetMaterial(g1)
	Duel.Overlay(c,g1)
end
function c23187256.filter(c,e,tp)
	return c:IsRankBelow(9) and c:IsAttackBelow(3000) and c:IsSetCard(0x48)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c23187256.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c23187256.filter,tp,LOCATION_EXTRA,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c23187256.gfilter(c,rank)
	return c:GetRank()==rank
end
function c23187256.operation(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local c=e:GetHandler()
	local g1=Duel.GetMatchingGroup(c23187256.filter,tp,LOCATION_EXTRA,0,nil,e,tp)
	local ct=c:GetOverlayGroup():GetClassCount(Card.GetCode)
	if ct>ft then ct=ft end
	if g1:GetCount()>0 and ct>0 then
		repeat
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
			local g2=g1:Select(tp,1,1,nil)
			local tc=g2:GetFirst()
			Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEUP)
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_DISABLE)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e1)
			local e2=Effect.CreateEffect(c)
			e2:SetType(EFFECT_TYPE_SINGLE)
			e2:SetCode(EFFECT_DISABLE_EFFECT)
			e2:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e2)
			g1:Remove(c23187256.gfilter,nil,tc:GetRank())
			ct=ct-1
		until g1:GetCount()==0 or ct==0 or not Duel.SelectYesNo(tp,aux.Stringid(23187256,1))
		Duel.SpecialSummonComplete()
		Duel.BreakEffect()
		c:RemoveOverlayCard(tp,1,1,REASON_EFFECT)
	end
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_CHANGE_DAMAGE)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetTargetRange(0,1)
	e3:SetValue(c23187256.val)
	e3:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e3,tp)
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e4:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e4:SetReset(RESET_PHASE+PHASE_END)
	e4:SetTargetRange(1,0)
	Duel.RegisterEffect(e4,tp)
end
function c23187256.val(e,re,dam,r,rp,rc)
	if bit.band(r,REASON_BATTLE)~=0 then
		return dam/2
	else return dam end
end
function c23187256.indfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ) and c:IsSetCard(0x48)
end
function c23187256.indcon(e)
	return Duel.IsExistingMatchingCard(c23187256.indfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,e:GetHandler())
end
