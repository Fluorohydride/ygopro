--FNo.0 未来皇ホープ
function c65305468.initial_effect(c)
	--xyz summon
	c:EnableReviveLimit()
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetCondition(c65305468.xyzcon)
	e1:SetOperation(c65305468.xyzop)
	e1:SetValue(SUMMON_TYPE_XYZ)
	c:RegisterEffect(e1)
	--indes
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e3:SetValue(1)
	c:RegisterEffect(e3)
	--avoid damage
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_NO_BATTLE_DAMAGE)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_AVOID_BATTLE_DAMAGE)
	e5:SetValue(1)
	c:RegisterEffect(e5)
	--control
	local e6=Effect.CreateEffect(c)
	e6:SetDescription(aux.Stringid(65305468,0))
	e6:SetCategory(CATEGORY_CONTROL)
	e6:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e6:SetCode(EVENT_DAMAGE_STEP_END)
	e6:SetTarget(c65305468.cttg)
	e6:SetOperation(c65305468.ctop)
	c:RegisterEffect(e6)
	--destroy replace
	local e7=Effect.CreateEffect(c)
	e7:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e7:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e7:SetCode(EFFECT_DESTROY_REPLACE)
	e7:SetRange(LOCATION_MZONE)
	e7:SetTarget(c65305468.reptg)
	c:RegisterEffect(e7)
	if not c65305468.xyz_filter then
		c65305468.xyz_filter=function(mc) return mc:IsType(TYPE_XYZ) and not mc:IsSetCard(0x48) and mc:IsCanBeXyzMaterial(c) end
	end
end
c65305468.xyz_number=0
c65305468.xyz_count=2
function c65305468.mfilter(c,xyzc)
	return c:IsFaceup() and c:IsType(TYPE_XYZ) and not c:IsSetCard(0x48) and c:IsCanBeXyzMaterial(xyzc)
end
function c65305468.xyzfilter1(c,g)
	return g:IsExists(c65305468.xyzfilter2,1,c,c:GetRank())
end
function c65305468.xyzfilter2(c,rk)
	return c:GetRank()==rk
end
function c65305468.xyzcon(e,c,og)
	if c==nil then return true end
	local tp=c:GetControler()
	local mg=nil
	if og then
		mg=og:Filter(c65305468.mfilter,nil,c)
	else
		mg=Duel.GetMatchingGroup(c65305468.mfilter,tp,LOCATION_MZONE,0,nil,c)
	end
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and mg:IsExists(c65305468.xyzfilter1,1,nil,mg)
end
function c65305468.xyzop(e,tp,eg,ep,ev,re,r,rp,c,og)
	local g=nil
	local sg=Group.CreateGroup()
	if og then
		g=og
		local tc=og:GetFirst()
		while tc do
			sg:Merge(tc:GetOverlayGroup())
			tc=og:GetNext()
		end
	else
		local mg=Duel.GetMatchingGroup(c65305468.mfilter,tp,LOCATION_MZONE,0,nil)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
		g=mg:FilterSelect(tp,c65305468.xyzfilter1,1,1,nil,mg)
		local tc1=g:GetFirst()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
		local g2=mg:FilterSelect(tp,c65305468.xyzfilter2,1,1,tc1,tc1:GetRank())
		local tc2=g2:GetFirst()
		g:Merge(g2)
		sg:Merge(tc1:GetOverlayGroup())
		sg:Merge(tc2:GetOverlayGroup())
	end
	Duel.SendtoGrave(sg,REASON_RULE)
	c:SetMaterial(g)
	Duel.Overlay(c,g)
end
function c65305468.cttg(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=e:GetHandler():GetBattleTarget()
	if chk==0 then return tc and tc:IsRelateToBattle() and tc:IsControlerCanBeChanged() end
	Duel.SetOperationInfo(0,CATEGORY_CONTROL,tc,1,0,0)
end
function c65305468.ctop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetHandler():GetBattleTarget()
	if tc:IsRelateToBattle() and not Duel.GetControl(tc,tp,PHASE_BATTLE,1) then
		if not tc:IsImmuneToEffect(e) and tc:IsAbleToChangeControler() then
			Duel.Destroy(tc,REASON_EFFECT)
		end
	end
end
function c65305468.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsReason(REASON_EFFECT) and c:CheckRemoveOverlayCard(tp,1,REASON_EFFECT) end
	if Duel.SelectYesNo(tp,aux.Stringid(65305468,1)) then
		c:RemoveOverlayCard(tp,1,1,REASON_EFFECT)
		return true
	else return false end
end
