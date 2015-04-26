--インフェルノイド・ティエラ
function c82734805.initial_effect(c)
	c:EnableReviveLimit()
	--fusion material
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCondition(c82734805.fscon)
	e1:SetOperation(c82734805.fsop)
	c:RegisterEffect(e1)
	--spsummon success
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(82734805,0))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetCondition(c82734805.con)
	e2:SetTarget(c82734805.tg)
	e2:SetOperation(c82734805.op)
	c:RegisterEffect(e2)
	--material check
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_MATERIAL_CHECK)
	e3:SetValue(c82734805.valcheck)
	e3:SetLabelObject(e2)
	c:RegisterEffect(e3)
end
c82734805.material_count=2
c82734805.material={14799437,23440231}
function c82734805.valcheck(e,c)
	local ct=e:GetHandler():GetMaterial():GetClassCount(Card.GetCode)
	e:GetLabelObject():SetLabel(ct)
end
function c82734805.con(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(e:GetHandler():GetSummonType(),SUMMON_TYPE_FUSION)==SUMMON_TYPE_FUSION 
end
function c82734805.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	local ct=e:GetLabel()
	local con3,con5,con8,con10=nil
	if ct>=3 then 
		con3=Duel.IsExistingMatchingCard(Card.IsAbleToGrave,tp,LOCATION_EXTRA,0,3,nil)
			and Duel.IsExistingMatchingCard(Card.IsAbleToGrave,tp,0,LOCATION_EXTRA,3,nil) 
	end
	if ct>=5 then
		con5=Duel.IsPlayerCanDiscardDeck(tp,3) and Duel.IsPlayerCanDiscardDeck(1-tp,3) 
	end
	if ct>=8 then
		con8=Duel.IsExistingMatchingCard(Card.IsFaceup,tp,LOCATION_REMOVED,0,1,nil) 
			and Duel.IsExistingMatchingCard(Card.IsFaceup,tp,0,LOCATION_REMOVED,1,nil)
	end
	if ct>=10 then
		con10=Duel.GetFieldGroupCount(tp,LOCATION_HAND,LOCATION_HAND)>0
	end
	if chk=0 then return con3 or con5 or con8 or con10 end
end
function c82734805.op(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabel()
	if ct>=3 then
		local g1=Duel.GetMatchingGroup(Card.IsAbleToGrave,tp,LOCATION_EXTRA,0,nil)
		local sg1=nil
		if g1:GetCount()>=3 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
			sg1=g1:Select(tp,3,3,nil)
		else sg1=g1 end
		local g2=Duel.GetMatchingGroup(Card.IsAbleToGrave,tp,0,LOCATION_EXTRA,nil)
		local sg2=nil
		if g2:GetCount()>=3 then
			Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TOGRAVE)
			sg2=g2:Select(1-tp,3,3,nil)
		else sg2=g2 end
		sg1:Merge(sg2)
		if sg1:GetCount()>0 then
			Duel.SendtoGrave(sg1,REASON_EFFECT)
		end
	end
	if ct>=5 then
		Duel.BreakEffect()
		Duel.DiscardDeck(tp,3,REASON_EFFECT)
		Duel.DiscardDeck(1-tp,3,REASON_EFFECT)
	end
	if ct>=8 then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g1=Duel.SelectMatchingCard(tp,Card.IsFaceup,tp,LOCATION_REMOVED,0,1,3,nil)
		Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TOGRAVE)
		local g2=Duel.SelectMatchingCard(1-tp,Card.IsFaceup,1-tp,LOCATION_REMOVED,0,1,3,nil)
		g1:Merge(g2)
		if g1:GetCount()>0 then
			Duel.SendtoGrave(g1,REASON_EFFECT+REASON_RETURN)
		end
	end
	if ct>=10 then
		Duel.BreakEffect()
		local g1=Duel.GetFieldGroup(tp,LOCATION_HAND,LOCATION_HAND)
		Duel.SendtoGrave(g1,REASON_EFFECT)
	end
end
--[[function c82734805.extg(tp)
	return Duel.IsExistingMatchingCard(Card.IsAbleToGrave,tp,LOCATION_EXTRA,0,3,nil)
		and Duel.IsExistingMatchingCard(Card.IsAbleToGrave,tp,0,LOCATION_EXTRA,3,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,6,PLAYER_ALL,LOCATION_EXTRA)
end
function c82734805.exop(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetMatchingGroup(Card.IsAbleToGrave,tp,LOCATION_EXTRA,0,nil)
	local sg1=nil
	if g1:GetCount()>=3 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		sg1=g1:Select(tp,3,3,nil)
	else sg1=g1 end
	local g2=Duel.GetMatchingGroup(Card.IsAbleToGrave,tp,0,LOCATION_EXTRA,nil)
	local sg2=nil
	if g2:GetCount()>=3 then
		Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TOGRAVE)
		sg2=g2:Select(1-tp,3,3,nil)
	else sg2=g2 end
	sg1:Merge(sg2)
	if sg1:GetCount()>0 then
		Duel.SendtoGrave(sg1,REASON_EFFECT)
	end
end
function c82734805.decktg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDiscardDeck(tp,3) and Duel.IsPlayerCanDiscardDeck(1-tp,3) end
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,nil,0,PLAYER_ALL,6)
end
function c82734805.deckop(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardDeck(tp,3,REASON_EFFECT)
	Duel.DiscardDeck(1-tp,3,REASON_EFFECT)
end
function c82734805.rettg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsFaceup,tp,LOCATION_REMOVED,0,1,nil) and Duel.IsExistingMatchingCard(Card.IsFaceup,tp,0,LOCATION_REMOVED,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,2,PLAYER_ALL,LOCATION_REMOVED)
end
function c82734805.retop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g1=Duel.SelectMatchingCard(tp,Card.IsFaceup,tp,LOCATION_REMOVED,0,1,3,nil)
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TOGRAVE)
	local g2=Duel.SelectMatchingCard(1-tp,Card.IsFaceup,1-tp,LOCATION_REMOVED,0,1,3,nil)
	g1:Merge(g2)
	if g1:GetCount()>0 then
		Duel.SendtoGrave(g1,REASON_EFFECT+REASON_RETURN)
	end
end
function c82734805.handtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_HAND,LOCATION_HAND)>0 end
	if Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)==0 then
		Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,1-tp,0)
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,1-tp,LOCATION_HAND)
	elseif Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)==0 then
		Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,tp,0)
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_HAND)
	else
		Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,PLAYER_ALL,0)
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,PLAYER_ALL,LOCATION_HAND)
	end
end
function c82734805.handop(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetFieldGroup(tp,LOCATION_HAND,LOCATION_HAND)
	Duel.SendtoGrave(g1,REASON_EFFECT)
end--]]
function c82734805.fcfilter1(c,code1,code2,g)
	local code=c:GetCode()
	return (code==code1 or code==code2) and g:IsExists(Card.IsSetCard,1,c,0xbb)
end
function c82734805.fcfilter2(c,code,g)
	return (c:IsCode(code) or c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE)) and g:IsExists(Card.IsSetCard,1,c,0xbb)
end
function c82734805.fcfilter3(c,g)
	return ((c:IsCode(14799437) or c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE)) and g:IsExists(Card.IsCode,1,c,23440231))
		or ((c:IsCode(23440231) or c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE)) and g:IsExists(Card.IsCode,1,c,14799437))
end
function c82734805.fscon(e,g,gc,chkf)
	if g==nil then return true end
	if gc then
		if gc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
			return g:IsExists(c82734805.fcfilter1,1,gc,14799437,23440231,g)
		elseif gc:IsCode(14799437) then
			return g:IsExists(c82734805.fcfilter2,1,gc,23440231,g)
		elseif gc:IsCode(23440231) then
			return g:IsExists(c82734805.fcfilter2,1,gc,14799437,g)
		elseif gc:IsSetCard(0xbb) then
			return g:IsExists(c82734805.fcfilter3,1,gc,g)
		end
	end
	local b1=0 local b2=0 local bs=0
	local g1=Group.CreateGroup() local g2=Group.CreateGroup() local fs=false
	local tc=g:GetFirst()
	while tc do
		if tc:IsCode(14799437) then
			b1=1 g1:AddCard(tc) if aux.FConditionCheckF(tc,chkf) then fs=true end end
		if tc:IsCode(23440231) then
			b2=1 g1:AddCard(tc) if aux.FConditionCheckF(tc,chkf) then fs=true end end
		if tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
			bs=1 g1:AddCard(tc) if aux.FConditionCheckF(tc,chkf) then fs=true end end
		if tc:IsSetCard(0xbb) then
			g2:AddCard(tc) if aux.FConditionCheckF(tc,chkf) then fs=true end end
		tc=g:GetNext()
	end
	if chkf~=PLAYER_NONE then
		return fs and b1+b2+bs>=2 and g2:GetCount()>=3-bs
	else
		return b1+b2+bs>=2 and g2:GetCount()>=3-bs
	end
end
function c82734805.fsop(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
	if gc then
		local sg=eg local g1=nil local g2=nil local g3=nil
		if gc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g1=eg:FilterSelect(tp,aux.FConditionFilter21,1,1,gc,14799437,23440231)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g2=eg:FilterSelect(tp,Card.IsSetCard,1,99,g1:GetFirst(),0xbb)
			g1:Merge(g2)
			Duel.SetFusionMaterial(g1)
			return
		elseif gc:IsCode(14799437) then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g1=eg:FilterSelect(tp,Card.IsCode,1,1,gc,23440231)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g2=eg:FilterSelect(tp,Card.IsSetCard,1,99,g1:GetFirst(),0xbb)
			g1:Merge(g2)
			Duel.SetFusionMaterial(g1)
			return
		elseif gc:IsCode(23440231) then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g1=eg:FilterSelect(tp,Card.IsCode,1,1,gc,14799437)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g2=eg:FilterSelect(tp,Card.IsSetCard,1,99,g1:GetFirst(),0xbb)
			g1:Merge(g2)
			Duel.SetFusionMaterial(g1)
			return
		elseif gc:IsSetCard(0xbb) then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g1=sg:FilterSelect(tp,aux.FConditionFilter21,1,1,gc,14799437,23440231)
			local code1=g1:GetCode() local code2=nil
			if code1==14799437 then code2=23440231 else code2=14799437 end
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g2=sg:FilterSelect(tp,Card.IsCode,1,1,gc,code2)
			g1:Merge(g2)
			sg:Sub(g1)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g3=sg:FilterSelect(tp,Card.IsSetCard,1,99,nil,0xbb)
			g1:Merge(g3)
			Duel.SetFusionMaterial(g1)
			return
		end
	end
	local sg1=Group.CreateGroup() local sg2=Group.CreateGroup() local fs=false
	local tc=eg:GetFirst()
	while tc do
		if tc:IsCode(14799437) or tc:IsCode(23440231) or tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then sg1:AddCard(tc) end
		if tc:IsSetCard(0xbb) then sg2:AddCard(tc) if Auxiliary.FConditionCheckF(tc,chkf) then fs=true end end
		tc=eg:GetNext()
	end
	local g1=nil local g2=nil local g3=nil local g4=nil
	if chkf~=PLAYER_NONE then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		if fs then g1=sg1:Select(tp,1,1,nil)
		else g1=sg1:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf) end
		local tc1=g1:GetFirst()
		if tc1:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
			sg1:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
		else sg1:Remove(Card.IsCode,nil,tc1:GetCode()) end
		sg2:RemoveCard(tc1)
		if Auxiliary.FConditionCheckF(tc1,chkf) or fs then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g2=sg1:Select(tp,1,1,nil)
		else g2=sg1:FilterSelect(tp,Auxiliary.FConditionCheckF,1,1,nil,chkf) end
		g1:Merge(g2)
		local tc2=g2:GetFirst()
		sg2:RemoveCard(tc2)
		if aux.FConditionCheckF(tc1,chkf) or aux.FConditionCheckF(tc2,chkf) then
			if e:GetHandler():GetFlagEffect(31444249)~=0 and Duel.SelectYesNo(tp,aux.Stringid(31444249,0)) then
				local dsg2=sg2:Filter(Card.IsLocation,nil,LOCATION_DECK)
				sg2:Sub(dsg2)
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				g3=dsg2:Select(tp,1,6-g1:FilterCount(Card.IsLocation,nil,LOCATION_DECK),nil)
				g1:Merge(g3)
				if sg2:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(31444249,1)) then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					g4=sg2:Select(tp,1,99,nil)
					g1:Merge(g4)
				end
			else
				if e:GetHandler():GetFlagEffect(31444249)~=0 then
					sg2:Remove(Card.IsLocation,nil,LOCATION_DECK)
				end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				g3=sg2:Select(tp,1,99,nil)
				g1:Merge(g3)
			end
		else
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g3=sg2:FilterSelect(tp,aux.FConditionCheckF,1,1,nil,chkf)
			g1:Merge(g3)
			sg2:RemoveCard(g3:GetFirst())
			if e:GetHandler():GetFlagEffect(31444249)~=0 and Duel.SelectYesNo(tp,aux.Stringid(31444249,0)) then
				local dsg2=sg2:Filter(Card.IsLocation,nil,LOCATION_DECK)
				sg2:Sub(dsg2)
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				g4=dsg2:Select(tp,1,6-g1:FilterCount(Card.IsLocation,nil,LOCATION_DECK),nil)
				g1:Merge(g4)
				if sg2:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(31444249,1)) then
					Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
					local g5=sg2:Select(tp,1,99,nil)
					g1:Merge(g5)
				end
			else
				if e:GetHandler():GetFlagEffect(31444249)~=0 then
					sg2:Remove(Card.IsLocation,nil,LOCATION_DECK)
				end
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				g4=sg2:Select(tp,1,99,nil)
				g1:Merge(g4)
			end
		end
		Duel.SetFusionMaterial(g1)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		g1=sg1:Select(tp,1,1,nil)
		local tc1=g1:GetFirst()
		if tc1:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then
			sg1:Remove(Card.IsHasEffect,nil,EFFECT_FUSION_SUBSTITUTE)
		else sg1:Remove(Card.IsCode,nil,tc1:GetCode()) end
		sg2:RemoveCard(tc1)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		g2=sg1:Select(tp,1,1,nil)
		sg2:RemoveCard(g2:GetFirst())
		g1:Merge(g2)
		if e:GetHandler():GetFlagEffect(31444249)~=0 and Duel.SelectYesNo(tp,aux.Stringid(31444249,0)) then
			local dsg2=sg2:Filter(Card.IsLocation,nil,LOCATION_DECK)
			sg2:Sub(dsg2)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g3=dsg2:Select(tp,1,6-g1:FilterCount(Card.IsLocation,nil,LOCATION_DECK),nil)
			g1:Merge(g3)
			if sg2:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(31444249,1)) then
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				g4=sg2:Select(tp,1,99,nil)
				g1:Merge(g4)
			end
		else
			if e:GetHandler():GetFlagEffect(31444249)~=0 then
				sg2:Remove(Card.IsLocation,nil,LOCATION_DECK)
			end
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g3=sg2:Select(tp,1,99,nil)
			g1:Merge(g3)
		end
		Duel.SetFusionMaterial(g1)
	end
end
